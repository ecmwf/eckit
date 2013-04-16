/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Sep 2012

#ifndef eckit_config_Compiler_H
#define eckit_config_Compiler_H

#include "eckit/machine.h"

#include "eckit/memory/NonCopyable.h"
#include "eckit/parser/StreamParser.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

class Compiler : public StreamParser {
public: // methods

    Compiler( istream& in );
    
    virtual ~Compiler();

    void consumeComment();
    string consumeToEOL();    
    string parseIdentifier();
    string parseValue( bool dontBreakOnSpace = false );
    StringList parseCondition();

protected:
    
    bool isIdentifier( char c );    
    
};

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit

#endif
