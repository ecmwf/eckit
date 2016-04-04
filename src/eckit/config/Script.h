/*
 * (C) Copyright 1996-2016 ECMWF.
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
#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"

#include "eckit/config/Compiler.h"

//-----------------------------------------------------------------------------

namespace eckit {
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
