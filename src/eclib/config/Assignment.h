/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_config_Assignment_H
#define eckit_config_Assignment_H

#include "eclib/machine.h"

#include "eclib/Types.h"

#include "eclib/config/Compiler.h"
#include "eclib/config/Statement.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

class Assignment : public Statement {

public: // methods
    
    Assignment( Compiler& c, Scope& scope );
    
    virtual ~Assignment();

    virtual void execute( const StringDict& in, StringDict& out );

    virtual void print( std::ostream& out );

private: // members
    
    std::string variable_;
    std::string value_;

};

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit

#endif
