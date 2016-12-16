/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_MD5RFC1321_H
#define eckit_utils_MD5RFC1321_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class MD5RFC1321 : private eckit::NonCopyable {

public:  // types

  typedef std::string digest_t;

public:  // types

  MD5RFC1321();

  explicit MD5RFC1321(const digest_t&);

  MD5RFC1321(const void* data, size_t len);

  ~MD5RFC1321();

  void add(const void*, long);

  void add(char x){ add(&x, sizeof(x)); }
  void add(unsigned char x){ add(&x, sizeof(x)); }

  void add(bool x){ add(&x, sizeof(x)); }

  void add(int x){ add(&x, sizeof(x)); }
  void add(unsigned int x){ add(&x, sizeof(x)); }

  void add(short x){ add(&x, sizeof(x)); }
  void add(unsigned short x){ add(&x, sizeof(x)); }

  void add(long x){ add(&x, sizeof(x)); }
  void add(unsigned long x){ add(&x, sizeof(x)); }

  void add(long long x){ add(&x, sizeof(x)); }
  void add(unsigned long long x){ add(&x, sizeof(x)); }

  void add(float x){ add(&x, sizeof(x)); }
  void add(double x){ add(&x, sizeof(x)); }

  void add(const std::string& x)  { add(x.c_str(),x.size()); }
  void add(const char* x) { add(std::string(x)); }
  
  /// for generic objects
  template <class T>
  void add(const T& x) { x.hash(*this); }

  template<class T>
  MD5RFC1321& operator<<(const T& x) { add(x); return *this; }

  operator std::string();

  digest_t digest() const;

private:  // types

  // Make sure this is not called with a pointer
  template<class T>
  void add(const T* x);
  void add(const void*);

  /// Double hashing
  void add(const MD5RFC1321& md5) { add(md5.digest()); }

private: // members

  mutable digest_t digest_;  ///< cached digest

  /* POINTER defines a generic pointer type */
  typedef unsigned char *POINTER;

  /* UINT4 defines a four byte word */
  typedef u_int32_t UINT4;

  /* MD5 context. */
  typedef struct {
    UINT4 state[4];                                   /* state (ABCD) */
    UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];                         /* input buffer */
  } MD5_CTX;

  mutable MD5_CTX s_;

  static void Init    (MD5_CTX *);
  static void Update  (MD5_CTX *, unsigned char *, unsigned int);
  static void Final   (unsigned char [16], MD5_CTX *);
  static void Transform (UINT4 [4], unsigned char [64]);
  static void Encode (unsigned char *, UINT4 *, unsigned int);
  static void Decode (UINT4 *, unsigned char *, unsigned int);

};

}  // end namespace eckit

#endif
