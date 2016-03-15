/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_FileReadPolicy_h
#define eckit_FileReadPolicy_h

#include "eckit/filesystem/PathName.h"

namespace eckit {

//-----------------------------------------------------------------------------

class DirectReadPolicy
{
  friend bool read(const DirectReadPolicy&, const PathName& path, std::stringstream& s );
};

//-----------------------------------------------------------------------------

class FileReadPolicy {

public:

  template <typename T>
  FileReadPolicy( T x ) : self_(new Model<T>(x))
  {}

  ~FileReadPolicy() { delete self_; }

  FileReadPolicy& operator=( const FileReadPolicy& x )
  {
    FileReadPolicy tmp(x);
    std::swap(tmp,*this);
    return *this;
  }

  friend bool read(const FileReadPolicy& x, const PathName& path, std::stringstream& s )
  {
    return x.self_->read_(path,s);
  }

private:

  struct Concept
  {
    virtual ~Concept() {}
    virtual bool read_( const PathName& path, std::stringstream& s ) const = 0;
  };

  template <typename T>
  struct Model : Concept
  {
    Model( T x ) : data_(x) {}
    virtual bool read_( const PathName& path, std::stringstream& s ) const
    {
      return read( data_, path, s );
    }
    T data_;
  };

  const Concept* self_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
