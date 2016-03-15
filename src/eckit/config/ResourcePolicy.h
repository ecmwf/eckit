/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ResourcePolicy_h
#define eckit_ResourcePolicy_h

#include "eckit/memory/ScopedPtr.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

namespace config { class Script; }

//-----------------------------------------------------------------------------

class ResourcePolicy {

public:

  template <typename T>
  ResourcePolicy( T x ) : self_(new Model<T>(x))
  {}

  ~ResourcePolicy() { delete self_; }

  ResourcePolicy& operator=( const ResourcePolicy& x )
  {
    ResourcePolicy tmp(x);
    std::swap(tmp,*this);
    return *this;
  }

  friend void configure( ResourcePolicy& x, config::Script& script )
  {
     x.self_->configure_(script);
  }

private:

  struct Concept
  {
    virtual ~Concept() {}
    virtual void configure_( config::Script& ) = 0;
  };

  template <typename T>
  struct Model : Concept
  {
    Model( T x ) : data_(x) {}
    virtual void configure_( config::Script& script )
    {
      configure( data_, script );
    }
    T data_;
  };

  Concept* self_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
