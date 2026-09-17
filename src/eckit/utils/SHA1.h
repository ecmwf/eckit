// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_utils_SHA1_H
#define eckit_utils_SHA1_H

#include "eckit/eckit.h"

#if eckit_HAVE_SSL
#include <openssl/sha.h>
#else
#error "eckit was not configured with OpenSSL, SHA1 is disabled. Use conditional eckit_HAVE_SSL from eckit/eckit.h"
#endif

#ifndef SHA_DIGEST_LENGTH
#define SHA_DIGEST_LENGTH 20
#endif

#include "eckit/utils/Hash.h"

namespace eckit {

class SHA1 : public Hash {

public:  // types

    SHA1();

    explicit SHA1(const char*);
    explicit SHA1(const std::string&);

    SHA1(const void* data, size_t len);

    ~SHA1() override;

    void reset() const override;

    digest_t compute(const void*, long) override;

    void update(const void*, long) override;

    digest_t digest() const override;

    template <class T>
    SHA1& operator<<(const T& x) {
        add(x);
        return *this;
    }

private:  // members

    mutable SHA_CTX ctx_;
};

}  // end namespace eckit

#endif
