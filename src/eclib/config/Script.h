/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_config_Script_H
#define eclib_config_Script_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"
#include "eclib/PathName.h"
#include "eclib/Types.h"

#include "eclib/config/Statement.h"
#include "eclib/config/Compiler.h"

//-----------------------------------------------------------------------------

namespace eclib {
namespace config {

//-----------------------------------------------------------------------------

class Block;

//-----------------------------------------------------------------------------

class Script : private NonCopyable {
    
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
    
    /// reads the contents of the stream into the script
    void readStream( istream& in );
    
    /// clears the contents of the script, effectively making it an empty script
    void clear();
 
private: // members
    
    typedef std::vector<Block*> BlockStore;
    
    BlockStore blocks_;
    
};

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eclib


#endif
