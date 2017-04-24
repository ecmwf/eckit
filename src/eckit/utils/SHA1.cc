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

#include "eckit/utils/SHA1.h"


namespace eckit {

SHA1::SHA1() : dirty_(true) {
    SHA1_Init(&ctx_);
}

SHA1::SHA1(const char* s) : dirty_(true)  {
    SHA1_Init(&ctx_);
    add( s, strlen(s) );
}

SHA1::SHA1(const std::string& s) : dirty_(true) {
    SHA1_Init(&ctx_);
    add( s.c_str(), s.size() );
}

SHA1::SHA1(const void* data, size_t len) {
    SHA1_Init(&ctx_);
    add( data, len );
}

SHA1::~SHA1() {}

void SHA1::add(const void* buffer, long length) {
    if (length > 0) {
        SHA1_Update(&ctx_, static_cast<const unsigned char*>(buffer), length);
        dirty_ = true;
    }
}

void SHA1::add(const char* x) { add(x, ::strlen(x)); }

SHA1::operator std::string() {
    return std::string(digest());
}

static const char* hex = "0123456789abcdef";

SHA1::digest_t SHA1::digest() const {

    if (dirty_) { // recompute the digest

        digest_[2*SHA_DIGEST_LENGTH] = 0; // null terminator

        unsigned char digest[SHA_DIGEST_LENGTH];
        SHA1_Final(digest, &ctx_);

        size_t j = 0;
        for(size_t i = 0; i<SHA_DIGEST_LENGTH; ++i) {
            digest_[j++] = hex[(digest[i] & 0xf0) >> 4];
            digest_[j++] = hex[(digest[i] & 0xf)];
        }

        dirty_ = false;
    }

    return digest_;
}

} // namespace eckit
