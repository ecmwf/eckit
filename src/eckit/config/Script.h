/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_config_Script_H
#define eckit_config_Script_H

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"

#include "eckit/config/Statement.h"
#include "eckit/config/Compiler.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

class Block;

//-----------------------------------------------------------------------------

class Script : private NonCopyable {

public:

  class ReadPolicy {
  public:

    template <typename T>
    ReadPolicy( T x ) : self_(new Model<T>(x))
    { }

    ~ReadPolicy()
    {
      delete self_;
    }

    ReadPolicy& operator=( const ReadPolicy& x )
    {
      ReadPolicy tmp(x);
      std::swap(tmp,*this);
      return *this;
    }

    friend bool read_file(const ReadPolicy& x, const PathName& path, Script& script )
    {
      x.self_->readFile_(path,script);
    }

  private:

    struct Concept {
      virtual ~Concept() {}
      virtual bool readFile_( const PathName&, Script& ) const = 0;
    };

    template <typename T>
    struct Model : Concept {
      Model( T x ) : data_(x) {}
      virtual bool readFile_( const PathName& path, Script& script ) const
      {
        return read_file( data_, path, script );
      }
      T data_;
    };

    const Concept* self_;
  };

public: // methods

    Script();
    Script( Compiler& c );

    virtual ~Script();

    void execute( const StringDict& in, StringDict& out );

    /// prints the script
    /// @param out stream where to print
    void print( std::ostream& out );

    /// reads the contents of the file into the script
    /// @param path to the file
    /// @returns true if the file was found and successfully parsed
    bool readFile( const PathName& path );

    bool readFile( const PathName& path, const ReadPolicy& policy )
    {
      return read_file(policy,path,*this);
    }

    /// reads the contents of the stream into the script
    void readStream( std::istream& in );

    /// clears the contents of the script, effectively making it an empty script
    void clear();

private: // members

    typedef std::vector<Block*> BlockStore;

    BlockStore blocks_;

};


//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit


#endif
