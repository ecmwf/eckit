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

namespace config {

class Block;

//-----------------------------------------------------------------------------

class Script : private NonCopyable {
    
public: // methods

    Script();
    Script( Compiler& c );
    
    virtual ~Script();
    
    void execute( const StringDict& in, StringDict& out );

    void print( std::ostream& out );
    
    void readFile( const PathName& path );
    void readStream( istream& in );
 
private: // members
    
    typedef std::vector<Block*> BlockStore;
    
    BlockStore blocks_;
    
};

//-----------------------------------------------------------------------------

} // namespace config

#endif
