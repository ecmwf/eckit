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
#include <cstring>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Hash : private eckit::NonCopyable {

public:  // types

  typedef std::string digest_t;

public:  // methods

  Hash();

  virtual ~Hash();

  virtual void reset() const = 0;

  virtual digest_t digest() const = 0;

  // for one shot, stateless computation of the hash of the buffer
  virtual digest_t compute(const void*, long) = 0;

  void add(char x){ update(&x, sizeof(x)); }
  void add(unsigned char x){ update(&x, sizeof(x)); }

  void add(bool x){ update(&x, sizeof(x)); }

  void add(int x){ update(&x, sizeof(x)); }
  void add(unsigned int x){ update(&x, sizeof(x)); }

  void add(short x){ update(&x, sizeof(x)); }
  void add(unsigned short x){ update(&x, sizeof(x)); }

  void add(long x){ update(&x, sizeof(x)); }
  void add(unsigned long x){ update(&x, sizeof(x)); }

  void add(long long x){ update(&x, sizeof(x)); }
  void add(unsigned long long x){ update(&x, sizeof(x)); }

  void add(float x){ update(&x, sizeof(x)); }
  void add(double x){ update(&x, sizeof(x)); }

  void add(const void* x, long size) { update(x, size); }

  void add(const std::string& x) { update(x.c_str(), x.size()); }
  void add(const char* x) { update(x, std::strlen(x)); }

  template<class T>
  Hash& operator<<(const T& x) { add(x); return *this; }

  operator std::string() { return digest(); }

protected: // methods

  // for incremental hashing
  virtual void update(const void*, long) = 0;

private:  // types

  // Make sure this is not called with a pointer
  template<class T> void add(const T* x);
  void add(const void*);

  /// Double hashing
  void add(const Hash& hash) { add(hash.digest()); }

protected: // members

  mutable digest_t digest_;  ///< cached digest

};

//----------------------------------------------------------------------------------------------------------------------

class NoHash : public Hash {

public:  // types

  NoHash();

  virtual ~NoHash();

  virtual void reset() const;

  virtual digest_t compute(const void*, long);

  virtual void update(const void*, long);

  virtual digest_t digest() const;

};

//----------------------------------------------------------------------------------------------------------------------

class HashFactory {

    std::string name_;
    virtual Hash* make() = 0;

  protected:

    HashFactory(const std::string &);
    virtual ~HashFactory();

  public:

    static bool has(const std::string& name);
    static void list(std::ostream &);
    static Hash* build(const std::string&);

};

template< class T>
class HashBuilder : public HashFactory {
    virtual Hash* make() {
        return new T();
    }
  public:
    HashBuilder(const std::string &name) : HashFactory(name) {}
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
