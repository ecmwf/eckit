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

#include "eckit/exception/Exceptions.h"

#include "eckit/utils/SHA1.h"

// Cray C++ compiler should not try to optimize this code
#if _CRAYC
#pragma _CRI noopt
#endif

namespace eckit {

SHA1::SHA1() {
    SHA1_Init(&ctx_);
}

SHA1::SHA1(const char* s) {
    SHA1_Init(&ctx_);
    add(s, strlen(s));
}

SHA1::SHA1(const std::string& s) {
    SHA1_Init(&ctx_);
    add(s.c_str(), s.size());
}

SHA1::SHA1(const void* data, size_t len) {
    SHA1_Init(&ctx_);
    add(data, len);
}

SHA1::~SHA1() {}

void SHA1::reset() const {
    SHA1_Init(&ctx_);
}

Hash::digest_t SHA1::compute(const void* buffer, long size) {
    SHA1 hash(buffer, size);
    return hash.digest();
}

void SHA1::update(const void* buffer, long length) {
    if (length > 0) {
        SHA1_Update(&ctx_, static_cast<const unsigned char*>(buffer), length);
        if (!digest_.empty())
            digest_ = digest_t();  // reset the digest
    }
}

static const char* hex = "0123456789abcdef";

SHA1::digest_t SHA1::digest() const {

    if (digest_.empty()) {  // recompute the digest

        unsigned char digest[SHA_DIGEST_LENGTH];
        SHA1_Final(digest, &ctx_);

        char x[2 * SHA_DIGEST_LENGTH];

        size_t j = 0;
        for (size_t i = 0; i < SHA_DIGEST_LENGTH; ++i) {
            x[j++] = hex[(digest[i] & 0xf0) >> 4];
            x[j++] = hex[(digest[i] & 0xf)];
        }

        digest_ = std::string(x, 2 * SHA_DIGEST_LENGTH);
    }

    return digest_;
}

namespace {
HashBuilder<SHA1> builder("SHA1");
}

}  // namespace eckit
