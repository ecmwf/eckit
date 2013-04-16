/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_config_Branch_H
#define eckit_config_Branch_H

#include "eclib/machine.h"

#include "eclib/ScopedPtr.h"

#include "eckit/config/Compiler.h"
#include "eckit/config/Statement.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

class Condition;

//-----------------------------------------------------------------------------

class Branch : public Statement {
public: // methods
    
    Branch(Compiler& c , Scope& scope);
    
    virtual ~Branch();

    virtual void execute( const StringDict& in, StringDict& out );

    virtual void print( std::ostream& out );
    
protected: // methods
    
    bool hasElse() { return else_; }
    
private: // members
    
    ScopedPtr<Condition> if_;
    ScopedPtr<Statement> then_;
    ScopedPtr<Statement> else_;
    
};

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit

#endif
