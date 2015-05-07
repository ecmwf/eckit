/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Baudouin Raoult
/// @date   May 2015

#ifndef eckit_container_CacheManager_h
#define eckit_container_CacheManager_h

#include <string>

#include "eckit/eckit.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Filesystem Cache Manager

class CacheManager : private NonCopyable {

    std::string name_;

 public:
  typedef std::string key_t;

 public:

  explicit CacheManager(const std::string& name);

  virtual bool get(const key_t& k, PathName& v) const;

  virtual PathName stage(const key_t& k) const;

  virtual bool commit(const key_t& k, const PathName& v);

  friend std::ostream& operator<<(std::ostream& s, const CacheManager& p) {
    p.print(s);
    return s;
  }

protected:

    const std::string& name() const { return name_; }

    virtual PathName entry(const key_t& k) const = 0;

private:

      virtual void print(std::ostream& s) const;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
