/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_MD4_H
#define eckit_utils_MD4_H

#include "eckit/eckit_config.h"

#ifdef ECKIT_HAVE_SSL
#include <openssl/md4.h>
#else
typedef int MD4_CTX;
#endif

#ifndef MD4_DIGEST_LENGTH
#define MD4_DIGEST_LENGTH 16
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class MD4 : private eckit::NonCopyable {

public:  // types

  typedef std::string digest_t;

public:  // types

  MD4();

  explicit MD4(const char*);
  explicit MD4(const std::string&);

  MD4(const void* data, size_t len);

  ~MD4();

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
  void add(const char* x);

  template<class T>
  MD4& operator<<(const T& x) { add(x); return *this; }

  operator std::string();

  digest_t digest() const;

private:  // types

  // Make sure this is not called with a pointer
  template<class T>
  void add(const T* x);
  void add(const void*);

  /// Double hashing
  void add(const MD4& hash) { add(hash.digest()); }

private: // members

  mutable MD4_CTX ctx_;
  mutable char digest_[2*MD4_DIGEST_LENGTH+1];
  mutable bool dirty_;

};

}  // end namespace eckit

#endif
