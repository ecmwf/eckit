/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_HashxxHash_H
#define eckit_utils_HashxxHash_H

/// @note This file is named HashxxHash and not simply xxHash not to clash with the included header xxhash.h
///       for case insensitive file systems

#include "eckit/eckit.h"

#ifdef ECKIT_HAVE_XXHASH
#include <xxhash.h>
#else
#error "eckit was not configured with xxHash, xxHash is disabled. Use conditional ECKIT_HAVE_XXHASH from eckit/eckit.h"
#endif

#include "eckit/utils/Hash.h"

namespace eckit {

class xxHash : public Hash {

public:  // types

  xxHash();

  explicit xxHash(const char*);
  explicit xxHash(const std::string&);

  xxHash(const void* data, size_t len);

  virtual ~xxHash();

  virtual void reset() const;

  virtual digest_t compute(const void*, long);

  virtual void update(const void*, long);

  virtual digest_t digest() const;

  template<class T>
  xxHash& operator<<(const T& x) { add(x); return *this; }

private: // members

  XXH64_state_t* ctx_;

};

}  // end namespace eckit

#endif
