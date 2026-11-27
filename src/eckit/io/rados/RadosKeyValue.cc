// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosKeyValue.h"

#include <rados/librados.h>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/log/CodeLocation.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

/// @note: keys are text. librados takes the key to resume a listing from as a C string, so a key
///   with a NUL byte could be stored but not listed.
void checkKey(const std::string& key) {
    if (key.find('\0') != std::string::npos) {
        throw eckit::UserError("Invalid RADOS key: it must not contain a NUL byte", Here());
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

RadosKeyValue::RadosKeyValue(const eckit::URI& uri) : object_(uri) {}

RadosKeyValue::RadosKeyValue(const std::string& pool, const std::string& nspace, const std::string& name) :
    object_(pool, nspace, name) {}

bool RadosKeyValue::exists() const {
    return object_.exists();
}

void RadosKeyValue::ensureCreated() {

    RadosWriteOp op;
    rados_write_op_create(op.op_, LIBRADOS_CREATE_IDEMPOTENT, nullptr);

    submit(op);
}

void RadosKeyValue::ensureDestroyed() {
    object_.ensureDestroyed();
}

void RadosKeyValue::put(const std::string& key, const void* data, size_t length) {

    checkKey(key);

    const char* keys[]       = {key.data()};
    const char* values[]     = {static_cast<const char*>(data)};
    const size_t keyLens[]   = {key.size()};
    const size_t valueLens[] = {length};

    RadosWriteOp op;
    rados_write_op_omap_set2(op.op_, keys, values, keyLens, valueLens, 1);

    submit(op);
}

void RadosKeyValue::remove(const std::string& key) {

    checkKey(key);

    const char* keys[]     = {key.data()};
    const size_t keyLens[] = {key.size()};

    RadosWriteOp op;
    rados_write_op_omap_rm_keys2(op.op_, keys, keyLens, 1);

    submit(op);
}

void RadosKeyValue::submit(RadosWriteOp& op) {
    RADOS_CALL(rados_write_op_operate(op.op_, RadosCluster::instance().ioCtx(object_), name().c_str(), nullptr, 0));
}

bool RadosKeyValue::has(const std::string& key) const {
    return read(key, nullptr);
}

std::optional<std::vector<char>> RadosKeyValue::get(const std::string& key) const {

    std::vector<char> value;
    if (!read(key, &value)) {
        return std::nullopt;
    }

    return value;
}

bool RadosKeyValue::read(const std::string& key, std::vector<char>* value) const {

    checkKey(key);

    const char* keys[]     = {key.data()};
    const size_t keyLens[] = {key.size()};

    RadosReadOp op;
    RadosIter found;
    int rc = 0;
    rados_read_op_omap_get_vals_by_keys2(op.op_, keys, 1, keyLens, &found.it_, &rc);

    /// @note: a key-value which does not exist has no keys
    if (!RADOS_CALL_FOUND(rados_read_op_operate(op.op_, RadosCluster::instance().ioCtx(object_), name().c_str(), 0))) {
        return false;
    }
    RADOS_CALL(rc);

    if (rados_omap_iter_size(found.it_) == 0) {
        return false;
    }

    if (value) {
        char* foundKey     = nullptr;
        char* foundValue   = nullptr;
        size_t foundKeyLen = 0;
        size_t foundLen    = 0;
        RADOS_CALL(rados_omap_get_next2(found.it_, &foundKey, &foundValue, &foundKeyLen, &foundLen));

        value->clear();
        if (foundLen > 0) {
            value->assign(foundValue, foundValue + foundLen);
        }
    }

    return true;
}

std::vector<std::string> RadosKeyValue::keys(int keysPerQuery) const {

    std::vector<std::string> res;

    const rados_ioctx_t ctx = RadosCluster::instance().ioCtx(object_);

    /// @note: each query resumes after the last key of the previous one
    std::string last;
    unsigned char more = 1;

    while (more) {

        RadosReadOp op;
        RadosIter page;
        int rc = 0;
        rados_read_op_omap_get_keys2(op.op_, last.empty() ? nullptr : last.c_str(), keysPerQuery, &page.it_, &more,
                                     &rc);

        /// @note: a key-value which does not exist has no keys
        if (!RADOS_CALL_FOUND(rados_read_op_operate(op.op_, ctx, name().c_str(), 0))) {
            break;
        }
        RADOS_CALL(rc);

        const unsigned int count = rados_omap_iter_size(page.it_);

        /// @note: without a key to resume after, another query would return the same
        if (count == 0) {
            break;
        }

        for (unsigned int i = 0; i < count; ++i) {
            char* key     = nullptr;
            size_t keyLen = 0;
            RADOS_CALL(rados_omap_get_next2(page.it_, &key, nullptr, &keyLen, nullptr));
            res.emplace_back(key, keyLen);
        }

        last = res.back();

        /// @note: such a key, written by another client, cannot be passed as the place to resume
        ///   from: the listing would restart from an earlier key and never end.
        if (more && last.find('\0') != std::string::npos) {
            throw eckit::UserError("Cannot list the keys of RADOS key-value " + str() +
                                       ": it holds a key with a NUL byte and more than " +
                                       std::to_string(keysPerQuery) + " keys",
                                   Here());
        }
    }

    return res;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
