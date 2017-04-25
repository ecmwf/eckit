/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_Hash_H
#define eckit_utils_Hash_H

#include "eckit/eckit_config.h"

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class Hash : private eckit::NonCopyable {

public:  // types

  typedef std::string digest_t;

public:  // methods

  Hash();

  virtual ~Hash();

  virtual digest_t digest() const = 0;

  virtual void add(const void*, long) = 0;

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

  void add(const std::string& x) { add(x.c_str(), x.size()); }

  template<class T>
  Hash& operator<<(const T& x) { add(x); return *this; }

  operator std::string() { return digest(); }

private:  // types

  // Make sure this is not called with a pointer
  template<class T> void add(const T* x);
  void add(const void*);

  /// Double hashing
  void add(const Hash& hash) { add(hash.digest()); }

protected: // members

  mutable digest_t digest_;  ///< cached digest

};

}  // end namespace eckit

#endif
