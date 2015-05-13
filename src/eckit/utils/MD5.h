/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_MD5_H
#define eckit_utils_MD5_H

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class MD5 : private eckit::NonCopyable {

public:  // types

  typedef std::string digest_t;

public:  // types

  MD5();

  explicit MD5(const digest_t&);

  MD5(const void* data, size_t len);

  ~MD5();

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

  /// Double hashing
  void add(const MD5& md5) { add(md5.digest()); }

  template<class T>
  MD5& operator<<(const T& x) { add(x); return *this; }

  operator std::string();

  digest_t digest() const;

private:  // types

  // Make sure this is not called with a pointer
  template<class T>
  void add(const T* x);
  void add(const void*);

  /// Double hashing
  void add(const MD5&);



  struct State {

    uint64_t size;

    unsigned long words[64];
    unsigned long word_count;

    unsigned char bytes[4];
    unsigned long byte_count;

    unsigned long h0;
    unsigned long h1;
    unsigned long h2;
    unsigned long h3;
  };

protected:  // methods

  static void md5_init(State* s);
  static void md5_add(State* s, const void* data, size_t len);
  static void md5_end(State* s, char* digest);
  static void md5_flush(State* s);

private: // members

  mutable State s_;
  mutable digest_t digest_;  ///< cached digest

};

}  // end namespace eckit

#endif
