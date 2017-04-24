/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>
#include <cstring>

#include "eckit/exception/Exceptions.h"

#include "eckit/utils/MD4.h"


namespace eckit {

MD4::MD4() {
    MD4_Init(&ctx_);
}

MD4::MD4(const char* s) {
    MD4_Init(&ctx_);
    add( s, strlen(s) );
}

MD4::MD4(const std::string& s) {
    MD4_Init(&ctx_);
    add( s.c_str(), s.size() );
}

MD4::MD4(const void* data, size_t len) {
    MD4_Init(&ctx_);
    add( data, len );
}

MD4::~MD4() {}

void MD4::add(const void* buffer, long length) {
    if (length > 0) {
        MD4_Update(&ctx_, static_cast<const unsigned char*>(buffer), length);
        if (!digest_.empty())
            digest_ = digest_t(); // reset the digest
    }
}

void MD4::add(const char* x) { add(x, ::strlen(x)); }

MD4::operator std::string() {
    return digest();
}

static const char* hex = "0123456789abcdef";

MD4::digest_t MD4::digest() const {

    if (digest_.empty()) { // recompute the digest

        unsigned char digest[MD4_DIGEST_LENGTH];
        MD4_Final(digest, &ctx_);

        char x[2*MD4_DIGEST_LENGTH];

        size_t j = 0;
        for(size_t i = 0; i<MD4_DIGEST_LENGTH; ++i) {
            x[j++] = hex[(digest[i] & 0xf0) >> 4];
            x[j++] = hex[(digest[i] & 0xf)];
        }

        digest_ = std::string(x, 2*MD4_DIGEST_LENGTH);
    }

    return digest_;
}

} // namespace eckit
