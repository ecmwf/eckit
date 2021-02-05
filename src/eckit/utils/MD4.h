/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_MD4_H
#define eckit_utils_MD4_H

#include "eckit/eckit.h"

#ifdef eckit_HAVE_SSL
#include <openssl/md4.h>
#else
#error "eckit was not configured with OpenSSL, SHA1 is disabled. Use conditional eckit_HAVE_SSL from eckit/eckit.h"
#endif

#ifndef MD4_DIGEST_LENGTH
#define MD4_DIGEST_LENGTH 16
#endif

#include "eckit/utils/Hash.h"

namespace eckit {

class MD4 : public Hash {

public:  // types
    MD4();

    explicit MD4(const char*);
    explicit MD4(const std::string&);

    MD4(const void* data, size_t len);

    virtual ~MD4() override;

    virtual void reset() const override;

    virtual digest_t compute(const void*, long) override;

    virtual void update(const void*, long) override;

    virtual digest_t digest() const override;

    template <class T>
    MD4& operator<<(const T& x) {
        add(x);
        return *this;
    }

private:  // members
    mutable MD4_CTX ctx_;
};

}  // end namespace eckit

#endif
