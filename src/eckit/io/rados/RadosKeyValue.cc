// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosKeyValue.h"

#include <rados/librados.h>

#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Log.h"
#include "eckit/serialisation/MemoryStream.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosKeyValue::RadosKeyValue(const eckit::URI& uri) {
    eckit::Tokenizer parse("/");
    std::vector<std::string> bits;
    parse(uri.name(), bits);

    if (bits.size() != 3) {
        throw eckit::UserError("Invalid RADOS URI '" + uri.asString() + "', expected rados:<pool>/<namespace>/<name>",
                               Here());
    }

    ns_  = eckit::RadosNamespace{bits[0], bits[1]};
    oid_ = bits[2];
}

RadosKeyValue::RadosKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid) :
    ns_(pool, nspace), oid_(oid) {}

eckit::URI RadosKeyValue::uri() const {
    if (ns_.name().empty()) {
        throw eckit::UserError(
            "The key-value " + str() + " is in the default namespace, which a RADOS URI cannot express", Here());
    }
    return eckit::URI{"rados", eckit::PathName(str())};
}

std::string RadosKeyValue::str() const {
    return ns_.pool().name() + '/' + ns_.name() + '/' + oid_;
}

bool RadosKeyValue::exists() const {
    return eckit::RadosCluster::instance().exists(*this);
}

std::unique_ptr<eckit::RadosAIO> RadosKeyValue::ensureCreatedAsync() const {

    eckit::RadosWriteOp op{};

    rados_write_op_create(op.op_, LIBRADOS_CREATE_IDEMPOTENT, NULL);

    rados_ioctx_t& ctx             = RadosCluster::instance().ioCtx(*this);
    std::unique_ptr<RadosAIO> comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(rados_aio_write_op_operate(op.op_, ctx, comp->comp_, name().c_str(), NULL,
                                          0  /// @note: flags
                                          ));

    return comp;
}

void RadosKeyValue::ensureCreated() const {
    auto comp = ensureCreatedAsync();
    comp->waitForComplete();
}

void RadosKeyValue::ensureDestroyed() const {
    eckit::RadosCluster::instance().removeIfExists(eckit::RadosObject{ns_.pool().name(), ns_.name(), oid_});
}

/// @note: librados cannot query the size of a value, so the value is fetched
eckit::Length RadosKeyValue::size(const std::string& key) const {

    char* found      = nullptr;
    size_t found_len = 0;
    auto val_mem     = get(key, found, found_len);

    return eckit::Length(found_len);
}

bool RadosKeyValue::has(const std::string& key) const {

    eckit::RadosReadOp op{};

    eckit::RadosIter iter{};

    const char* key_c = key.c_str();
    size_t key_len    = key.size();

    int rc;

    rados_read_op_omap_get_vals_by_keys2(op.op_, (char const* const*)&(key_c),
                                         1,  /// @note: number of keys/values requested
                                         &(key_len), &(iter.it_), &rc);

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    eckit::RadosAIO comp{};

    RADOS_CALL(rados_aio_read_op_operate(op.op_, ctx, comp.comp_, name().c_str(),
                                         0  /// @note: flags
                                         ));

    /// @note: a key-value which does not exist has no keys
    if (!comp.waitForCompleteIfFound()) {
        return false;
    }

    ASSERT(rc == 0);

    return (rados_omap_iter_size(iter.it_) == 1);
}

std::unique_ptr<eckit::RadosAIO> RadosKeyValue::putAsync(const std::string& key, const void* buf, const long& buflen,
                                                         long& res) const {

    eckit::RadosWriteOp op{};

    const char* key_c = key.c_str();
    size_t key_len    = key.size();
    size_t val_len    = buflen;

    rados_write_op_omap_set2(op.op_, (char const* const*)&(key_c), (char const* const*)&(buf), &(key_len), &(val_len),
                             1  /// @note: number of keys/values provided
    );

    rados_ioctx_t& ctx                    = RadosCluster::instance().ioCtx(*this);
    std::unique_ptr<eckit::RadosAIO> comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(rados_aio_write_op_operate(op.op_, ctx, comp->comp_, name().c_str(), NULL,
                                          0  /// @note: flags
                                          ));

    res = buflen;

    return comp;
}

long RadosKeyValue::put(const std::string& key, const void* buf, const long& len) const {

    long res;

    auto comp = putAsync(key, buf, len, res);

    comp->waitForComplete();

    return res;
}

/// @note: returns a RadosIter holding the value data, and sets val to point to the data in it
std::unique_ptr<eckit::RadosIter> RadosKeyValue::get(const std::string& key, char*& val, size_t& len) const {

    eckit::RadosReadOp op{};

    auto iter = std::make_unique<eckit::RadosIter>();

    const char* key_c = key.c_str();
    size_t key_len    = key.size();

    int rc;

    rados_read_op_omap_get_vals_by_keys2(op.op_, (char const* const*)&(key_c),
                                         1,  /// @note: number of keys/values requested
                                         &(key_len), &(iter->it_), &rc);

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    eckit::RadosAIO comp{};

    RADOS_CALL(rados_aio_read_op_operate(op.op_, ctx, comp.comp_, name().c_str(),
                                         0  /// @note: flags
                                         ));

    comp.waitForComplete();

    ASSERT(rc == 0);

    if (rados_omap_iter_size(iter->it_) != 1) {
        throw eckit::RadosEntityNotFoundException("Key '" + key + "' not found in KeyValue with name " + oid_);
    }

    char* found_key      = nullptr;
    size_t found_key_len = 0;

    RADOS_CALL(rados_omap_get_next2(iter->it_, &found_key, &val, &found_key_len, &len));

    ASSERT(found_key);
    if (len > 0) {
        ASSERT(val);
    }
    ASSERT(key.size() == found_key_len);
    ASSERT(std::memcmp(key.c_str(), found_key, found_key_len) == 0);

    return iter;
}

long RadosKeyValue::get(const std::string& key, void* buf, const long& len) const {

    char* found      = nullptr;
    size_t found_len = 0;
    auto val_mem     = get(key, found, found_len);

    if (len < 0 || found_len > static_cast<size_t>(len)) {
        std::ostringstream oss;
        oss << "Value of key '" << key << "' in KeyValue " << str() << " has " << found_len
            << " bytes, which do not fit in the buffer of " << len << " bytes";
        throw eckit::BadValue(oss.str(), Here());
    }

    if (found_len > 0) {
        std::memcpy(buf, found, found_len);
    }

    return found_len;
}

eckit::MemoryStream RadosKeyValue::getMemoryStream(std::vector<char>& value, const std::string& key,
                                                   const std::string& kvTitle) const {

    char* found  = nullptr;
    size_t len   = 0;
    auto val_mem = get(key, found, len);

    value.resize(len);
    if (len > 0) {
        std::memcpy(value.data(), found, len);
    }

    static const char empty = '\0';
    return {len > 0 ? value.data() : &empty, len};
}

std::unique_ptr<eckit::RadosAIO> RadosKeyValue::removeAsync(const std::string& key) const {

    eckit::RadosWriteOp op{};

    const char* key_c = key.c_str();
    size_t key_len    = key.size();

    rados_write_op_omap_rm_keys2(op.op_, (char const* const*)&(key_c), &(key_len),
                                 1  /// @note: number of keys/values provided
    );

    rados_ioctx_t& ctx                    = RadosCluster::instance().ioCtx(*this);
    std::unique_ptr<eckit::RadosAIO> comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(rados_aio_write_op_operate(op.op_, ctx, comp->comp_, name().c_str(), NULL,
                                          0  /// @note: flags
                                          ));

    return comp;
}

void RadosKeyValue::remove(const std::string& key) const {

    auto comp = removeAsync(key);

    comp->waitForComplete();
}

std::vector<std::string> RadosKeyValue::keys(int keysPerQuery) const {

    std::vector<std::string> res;

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);

    int rc;
    unsigned char more = 1;

    /// @note: marker to resume the listing. each iteration advances rather than restarting from the beginning.
    std::string startAfter;

    while (more) {

        eckit::RadosReadOp op{};

        eckit::RadosIter iter{};

        rados_read_op_omap_get_keys2(op.op_,
                                     startAfter.empty() ? NULL : startAfter.c_str(),  /// @note: start search after
                                     keysPerQuery, &(iter.it_), &more, &rc);

        eckit::RadosAIO comp{};

        RADOS_CALL(rados_aio_read_op_operate(op.op_, ctx, comp.comp_, name().c_str(),
                                             0  /// @note: flags
                                             ));

        comp.waitForComplete();

        ASSERT(rc == 0);

        char* key = nullptr;
        size_t key_len;

        const unsigned int page = rados_omap_iter_size(iter.it_);

        for (unsigned int i = 0; i < page; ++i) {

            RADOS_CALL(rados_omap_get_next2(iter.it_, &key,
                                            NULL,  /// @note: where to store the value
                                            &key_len,
                                            NULL  /// @note: where to store the value length
                                            ));

            res.emplace_back(key, key + key_len);
        }

        /// @note: If this page was empty while more keys are reported, there is no valid
        ///   marker to advance with, so stop to avoid an infinite loop.
        if (page == 0) {
            break;
        }
        startAfter = res.back();
    }

    return res;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
