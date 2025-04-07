/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_MD5_H
#define eckit_utils_MD5_H

#ifndef MD5_DIGEST_LENGTH
#define MD5_DIGEST_LENGTH 16
#endif

#include "eckit/utils/Hash.h"

namespace eckit {

class MD5 : public Hash {

public:  // types

    MD5();

    explicit MD5(const char*);
    explicit MD5(const std::string&);

    MD5(const void* data, size_t len);

    ~MD5() override;

    void reset() const override;

    digest_t compute(const void*, long) override;

    void update(const void*, long) override;

    digest_t digest() const override;

    // Due to C++ name lookup rules, the base class interface needs to be
    // replicated here to be able to add the template overload, which would
    // otherwise shadow those methods.

    void add(char x) { update(&x, sizeof(x)); }
    void add(unsigned char x) { update(&x, sizeof(x)); }

    void add(bool x) { update(&x, sizeof(x)); }

    void add(int x) { update(&x, sizeof(x)); }
    void add(unsigned int x) { update(&x, sizeof(x)); }

    void add(short x) { update(&x, sizeof(x)); }
    void add(unsigned short x) { update(&x, sizeof(x)); }

    void add(long x) { update(&x, sizeof(x)); }
    void add(unsigned long x) { update(&x, sizeof(x)); }

    void add(long long x) { update(&x, sizeof(x)); }
    void add(unsigned long long x) { update(&x, sizeof(x)); }

    void add(float x) { update(&x, sizeof(x)); }
    void add(double x) { update(&x, sizeof(x)); }

    void add(const void* x, long size) { update(x, size); }

    void add(const std::string& x) { update(x.c_str(), x.size()); }
    void add(const char* x) { update(x, std::strlen(x)); }

    // for generic objects
    template <class T>
    void add(const T& x) {
        x.hash(*this);
    }

    template <class T>
    MD5& operator<<(const T& x) {
        add(x);
        return *this;
    }

    void numericalDigest(unsigned char out[MD5_DIGEST_LENGTH]) const;

private:  // members

    mutable digest_t digest_;  ///< cached digest

    /* POINTER defines a generic pointer type */
    typedef unsigned char* POINTER;

    /* UINT4 defines a four byte word */
    typedef uint32_t UINT4;

    /* MD5 context. */
    typedef struct {
        UINT4 state[4];           /* state (ABCD) */
        UINT4 count[2];           /* number of bits, modulo 2^64 (lsb first) */
        unsigned char buffer[64]; /* input buffer */
    } MD5_CTX;

    mutable MD5_CTX s_;

    static void Init(MD5_CTX*);
    static void Update(MD5_CTX*, const unsigned char*, unsigned int);
    static void Final(unsigned char[16], MD5_CTX*);
    static void Transform(UINT4[4], const unsigned char[64]);
    static void Encode(unsigned char*, UINT4*, unsigned int);
    static void Decode(UINT4*, const unsigned char*, unsigned int);
};

}  // end namespace eckit

#endif
