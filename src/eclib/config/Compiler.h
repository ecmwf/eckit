/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_config_Compiler_H
#define eclib_config_Compiler_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"
#include "eclib/StreamParser.h"
#include "eclib/Types.h"

namespace config {

class Compiler : public StreamParser {
public: // methods

    Compiler( istream& in );
    
    virtual ~Compiler();

    void consumeComment();
    string consumeToEOL();    
    string parseIdentifier();
    string parseValue();
    StringList parseCondition();

protected:
    
    bool isIdentifier( char c );    
    
};

} // namespace config

#endif
