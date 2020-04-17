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

#include "eckit/utils/MD4.h"

// Cray C++ compiler should not try to optimize this code
#if _CRAYC
#pragma _CRI noopt
#endif

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static const char* hex = "0123456789abcdef";
static std::string toString(unsigned char digest[MD4_DIGEST_LENGTH]) {

    char x[2 * MD4_DIGEST_LENGTH];

    size_t j = 0;
    for (size_t i = 0; i < MD4_DIGEST_LENGTH; ++i) {
        x[j++] = hex[(digest[i] & 0xf0) >> 4];
        x[j++] = hex[(digest[i] & 0xf)];
    }

    return std::string(x, 2 * MD4_DIGEST_LENGTH);
}

//----------------------------------------------------------------------------------------------------------------------

MD4::~MD4() {}

void MD4::reset() const {
    MD4_Init(&ctx_);
}

Hash::digest_t MD4::compute(const void* buffer, long size) {
    MD4_CTX s;
    MD4_Init(&s);
    MD4_Update(&s, static_cast<const unsigned char*>(buffer), size);
    unsigned char digest[MD4_DIGEST_LENGTH];
    MD4_Final(digest, &s);
    return toString(digest);
}

MD4::MD4() {
    MD4_Init(&ctx_);
}

MD4::MD4(const char* s) {
    MD4_Init(&ctx_);
    add(s, strlen(s));
}

MD4::MD4(const std::string& s) {
    MD4_Init(&ctx_);
    add(s.c_str(), s.size());
}

MD4::MD4(const void* data, size_t len) {
    MD4_Init(&ctx_);
    add(data, len);
}

void MD4::update(const void* buffer, long length) {
    if (length > 0) {
        MD4_Update(&ctx_, static_cast<const unsigned char*>(buffer), length);
        if (!digest_.empty())
            digest_ = digest_t();  // reset the digest
    }
}

MD4::digest_t MD4::digest() const {

    if (digest_.empty()) {  // recompute the digests
        unsigned char digest[MD4_DIGEST_LENGTH];
        MD4_Final(digest, &ctx_);
        digest_ = toString(digest);
    }

    return digest_;
}

namespace {
HashBuilder<MD4> builder("MD4");
}

}  // namespace eckit
