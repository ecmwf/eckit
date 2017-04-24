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

MD4::MD4() : dirty_(true) {
    MD4_Init(&ctx_);
}

MD4::MD4(const char* s) : dirty_(true)  {
    MD4_Init(&ctx_);
    add( s, strlen(s) );
}

MD4::MD4(const std::string& s) : dirty_(true) {
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
        dirty_ = true;
    }
}

void MD4::add(const char* x) { add(x, ::strlen(x)); }

MD4::operator std::string() {
    return std::string(digest());
}

static const char* hex = "0123456789abcdef";

MD4::digest_t MD4::digest() const {

    if (dirty_) { // recompute the digest

        digest_[2*MD4_DIGEST_LENGTH] = 0; // null terminator

        unsigned char digest[MD4_DIGEST_LENGTH];
        MD4_Final(digest, &ctx_);

        size_t j = 0;
        for(size_t i = 0; i<MD4_DIGEST_LENGTH; ++i) {
            digest_[j++] = hex[(digest[i] & 0xf0) >> 4];
            digest_[j++] = hex[(digest[i] & 0xf)];
        }

        dirty_ = false;
    }

    return digest_;
}

} // namespace eckit
