/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/io/rados/RadosKeyValue.h"

#include "eckit/utils/Tokenizer.h"

namespace eckit {

RadosKeyValue::RadosKeyValue(const eckit::URI& uri) {
    eckit::Tokenizer parse("/");
    std::vector<std::string> bits;
    parse(uri.name(), bits);

    ASSERT(bits.size() == 3);

    ns_ = eckit::RadosNamespace{bits[0], bits[1]};
    oid_  = bits[2];
}

RadosKeyValue::RadosKeyValue(const std::string& pool, const std::string& nspace, const std::string& oid) :
    ns_(pool, nspace), oid_(oid) {}

std::string RadosKeyValue::str() const {
    return ns_.pool().name() + '/' + ns_.name() + '/' + oid_;
}

bool RadosKeyValue::exists() const {
    return eckit::RadosCluster::instance().exists(*this);
}

std::unique_ptr<eckit::RadosAIO> RadosKeyValue::ensureCreated(bool ensure_persisted) {

    eckit::RadosWriteOp op{};

    rados_write_op_create(
        op.op_, 
        LIBRADOS_CREATE_IDEMPOTENT, 
        NULL
    );

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    std::unique_ptr<RadosAIO> comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(
        rados_aio_write_op_operate(
            op.op_, 
            ctx, 
            comp->comp_, 
            name().c_str(), 
            NULL, 
            0  /// @note: flags
        )
    );

    if (ensure_persisted) {
        RADOS_CALL(rados_aio_wait_for_safe(comp->comp_));
    } else {
        RADOS_CALL(rados_aio_wait_for_complete(comp->comp_));
    }

    return comp;

}

void RadosKeyValue::ensureCreated() {

    auto res = ensureCreated(false);

}

void RadosKeyValue::ensureDestroyed() {

    try {

        eckit::RadosCluster::instance().remove(*this);

    } catch (eckit::RadosEntityNotFoundException& e) {}

}

eckit::Length RadosKeyValue::size(const std::string& key) const {

    NOTIMP;
    /// @note: not implemented as it would require retrieving the actual value 
    ///   with a read_op_omap_get_vals_by_keys2 call. Better force the user code
    ///   to use the available get methods.

}

bool RadosKeyValue::has(const std::string& key) const {

    eckit::RadosReadOp op{};

    eckit::RadosIter iter{};

    const char *key_c = key.c_str();
    size_t key_len = key.size();

    int rc;

    rados_read_op_omap_get_vals_by_keys2(
        op.op_,
        (char const * const *) &(key_c), 
        1,  /// @note: number of keys/values requested
        &(key_len), 
        &(iter.it_),
        &rc
    );

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    eckit::RadosAIO comp{};

    RADOS_CALL(
        rados_aio_read_op_operate(
            op.op_, 
            ctx, 
            comp.comp_, 
            name().c_str(),
            0  /// @note: flags
        )
    );

    RADOS_CALL(rados_aio_wait_for_complete(comp.comp_));

    ASSERT(rc == 0);

    return (rados_omap_iter_size(iter.it_) == 1);

}

std::unique_ptr<eckit::RadosAIO> RadosKeyValue::put(const std::string& key, const void* buf, const long& buflen, long& res, bool ensure_persisted) {

    eckit::RadosWriteOp op{};

    const char *key_c = key.c_str();
    size_t key_len = key.size();
    size_t val_len = buflen;

    rados_write_op_omap_set2(
        op.op_, 
        (char const * const *) &(key_c), 
        (char const * const *) &(buf), 
        &(key_len), 
        &(val_len), 
        1  /// @note: number of keys/values provided
    );

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    std::unique_ptr<eckit::RadosAIO> comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(
        rados_aio_write_op_operate(
            op.op_, 
            ctx, 
            comp->comp_, 
            name().c_str(), 
            NULL, 
            0  /// @note: flags
        )
    );

    if (ensure_persisted) {
        RADOS_CALL(rados_aio_wait_for_safe(comp->comp_));
    } else {
        RADOS_CALL(rados_aio_wait_for_complete(comp->comp_));
    }

    res = buflen;

    return comp;

}

long RadosKeyValue::put(const std::string& key, const void* buf, const long& len) {

    long res;

    put(key, buf, len, res, false);

    return res;

}

/// @note: returns a RadosIter holding the value data, and sets val to point to the data in it
std::unique_ptr<eckit::RadosIter> RadosKeyValue::get(const std::string& key, char*& val, size_t& len) const { 

    eckit::RadosReadOp op{};

    std::unique_ptr<eckit::RadosIter> iter = std::make_unique<eckit::RadosIter>();

    const char *key_c = key.c_str();
    size_t key_len = key.size();

    int rc;

    rados_read_op_omap_get_vals_by_keys2(
        op.op_,
        (char const * const *) &(key_c), 
        1,  /// @note: number of keys/values requested
        &(key_len), 
        &(iter->it_),
        &rc
    );

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    eckit::RadosAIO comp{};

    RADOS_CALL(
        rados_aio_read_op_operate(
            op.op_, 
            ctx, 
            comp.comp_, 
            name().c_str(),
            0  /// @note: flags
        )
    );

    RADOS_CALL(rados_aio_wait_for_complete(comp.comp_));

    ASSERT(rc == 0);

    if (rados_omap_iter_size(iter->it_) != 1)
        throw eckit::RadosEntityNotFoundException("Key '" + key + "' not found in KeyValue with name " + oid_);

    char *found_key;
    size_t found_key_len;

    RADOS_CALL(rados_omap_get_next2(iter->it_, &found_key, &val, &found_key_len, &len));

    ASSERT(found_key);
    ASSERT(val);
    ASSERT(len > 0);
    ASSERT(key.size() == found_key_len);
    ASSERT(std::memcmp(key.c_str(), found_key, found_key_len) == 0);

    return iter;

}

long RadosKeyValue::get(const std::string& key, void* buf, const long& len) const { 

    char *found_val;
    size_t found_val_len;
    auto val_mem = get(key, found_val, found_val_len);

    ASSERT(found_val_len <= len);

    std::memcpy(buf, found_val, found_val_len);

    return found_val_len;

}

eckit::MemoryStream RadosKeyValue::getMemoryStream(std::vector<char>& v, const std::string& key, const std::string& kvTitle) const {

    char *found_val;
    size_t found_val_len;
    auto val_mem = get(key, found_val, found_val_len);

    v.resize(found_val_len);
    std::memcpy(&v[0], found_val, found_val_len);

    return eckit::MemoryStream(&v[0], found_val_len);

}

std::unique_ptr<eckit::RadosAIO> RadosKeyValue::remove(const std::string& key, bool ensure_persisted) {

    eckit::RadosWriteOp op{};

    const char *key_c = key.c_str();
    size_t key_len = key.size();

    rados_write_op_omap_rm_keys2(
        op.op_, 
        (char const * const *) &(key_c), 
        &(key_len),
        1  /// @note: number of keys/values provided
    );

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);
    std::unique_ptr<eckit::RadosAIO> comp = std::make_unique<eckit::RadosAIO>();

    RADOS_CALL(
        rados_aio_write_op_operate(
            op.op_, 
            ctx, 
            comp->comp_, 
            name().c_str(), 
            NULL, 
            0  /// @note: flags
        )
    );

    if (ensure_persisted) {
        RADOS_CALL(rados_aio_wait_for_safe(comp->comp_));
    } else {
        RADOS_CALL(rados_aio_wait_for_complete(comp->comp_));
    }

    return comp;

}

void RadosKeyValue::remove(const std::string& key) {

    remove(key, false);

}

std::vector<std::string> RadosKeyValue::keys(int keysPerQuery) const {

    std::vector<std::string> res;

    rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(*this);

    int rc;
    unsigned char more = 1;

    while (more) {

        eckit::RadosReadOp op{};

        eckit::RadosIter iter{};

        rados_read_op_omap_get_keys2(
            op.op_, 
            NULL,  /// @note: start search after
            keysPerQuery, 
            &(iter.it_), 
            &more, 
            &rc
        );

        eckit::RadosAIO comp{};

        RADOS_CALL(
            rados_aio_read_op_operate(
                op.op_, 
                ctx, 
                comp.comp_, 
                name().c_str(), 
                0  /// @note: flags
            )
        );

        RADOS_CALL(rados_aio_wait_for_complete(comp.comp_));

        ASSERT(rc == 0);

        char *key;
        size_t key_len;

        for (int i = 0; i < rados_omap_iter_size(iter.it_); ++i) {

            RADOS_CALL(
                rados_omap_get_next2(
                    iter.it_, 
                    &key, 
                    NULL,  /// @note: where to store the value
                    &key_len, 
                    NULL  /// @note: where to store the value length
                )
            );

            res.push_back(std::string(key, key + key_len));

        }

    }

    return res;

}

}  // namespace eckit