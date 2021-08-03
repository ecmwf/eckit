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
#include <iostream>

#define XXH_INLINE_ALL
#include "eckit/contrib/xxhash/xxhash.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/xxHashing.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct xxHash::Context {
    XXH64_state_t* state_;

    Context() {
        state_ = XXH64_createState();
        reset();
    }

    ~Context() {
        XXH64_freeState(state_);
    }

    void reset() {
        XXH64_reset(state_, 0);
    }

    void update(const void* buffer, long length) {
        XXH64_update(state_, buffer, size_t(length));
    }

    std::string digest() {
        return toString(XXH64_digest(state_));
    }

    static std::string compute(const void* buffer, long length) {
        return toString(XXH64(buffer, size_t(length), 0));
    }

    static std::string toString(XXH64_hash_t hash) {
        static const char* hex = "0123456789abcdef";
        char buffer[16];
        for (int i = 16; i--;) {
            buffer[i] = hex[hash & 15];
            hash >>= 4;
        }
        return std::string(buffer, buffer + 16);
    }
};

//----------------------------------------------------------------------------------------------------------------------

xxHash::xxHash() {
    ctx_.reset(new Context());
}

xxHash::xxHash(const char* s) {
    ctx_.reset(new Context());
    add(s, strlen(s));
}

xxHash::xxHash(const std::string& s) {
    ctx_.reset(new Context());
    add(s.c_str(), s.size());
}

xxHash::xxHash(const void* data, size_t len) {
    ctx_.reset(new Context());
    add(data, len);
}

xxHash::~xxHash() {}

void xxHash::reset() const {
    ctx_->reset();
}

Hash::digest_t xxHash::compute(const void* buffer, long size) {
    return Context::compute(buffer, size);
}

void xxHash::update(const void* buffer, long length) {
    if (length > 0) {
        ctx_->update(buffer, length);
        if (!digest_.empty())
            digest_ = digest_t();  // reset the digest
    }
}

xxHash::digest_t xxHash::digest() const {
    if (digest_.empty()) {  // recompute the digest
        digest_ = ctx_->digest();
    }
    return digest_;
}

//----------------------------------------------------------------------------------------------------------------------

namespace {
HashBuilder<xxHash> deprecated_builder("xxHash");
HashBuilder<xxHash> builder("xxh64");
}  // namespace

//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
