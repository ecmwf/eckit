/*
 * (C) Copyright 1996-2017 ECMWF.
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

  virtual ~MD5();

  virtual void reset() const;

  virtual digest_t compute(const void*, long);

  virtual void update(const void*, long);

  virtual digest_t digest() const;

  template<class T>
  MD5& operator<<(const T& x) { add(x); return *this; }

private: // members

  mutable digest_t digest_;  ///< cached digest

  /* POINTER defines a generic pointer type */
  typedef unsigned char *POINTER;

  /* UINT4 defines a four byte word */
  typedef uint32_t UINT4;

  /* MD5 context. */
  typedef struct {
    UINT4 state[4];                                   /* state (ABCD) */
    UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];                         /* input buffer */
  } MD5_CTX;

  mutable MD5_CTX s_;

  static void Init    (MD5_CTX *);
  static void Update  (MD5_CTX *, const unsigned char *, unsigned int);
  static void Final   (unsigned char [16], MD5_CTX *);
  static void Transform (UINT4 [4], const unsigned char [64]);
  static void Encode (unsigned char *, UINT4 *, unsigned int);
  static void Decode (UINT4 *, const unsigned char *, unsigned int);

};

}  // end namespace eckit

#endif