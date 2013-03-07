/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_config_Statement_H
#define eckit_config_Statement_H

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"
#include "eclib/Types.h"

#include "eclib/config/Scope.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

class Statement : private NonCopyable {

public: // methods

    Statement( Scope& scope );
    
    virtual ~Statement();
    
    virtual void execute( const StringDict& in, StringDict& out ) = 0;

    virtual void print( std::ostream& out ) = 0;
    
    Scope& scope() { return scope_; }
    
private: // members
    
    Scope& scope_;
    
};

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit

#endif
