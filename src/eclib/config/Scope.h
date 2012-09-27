/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eclib_config_Scope_H
#define eclib_config_Scope_H

#include "eclib/machine.h"

namespace config {

class Function;

//-----------------------------------------------------------------------------

class Scope {
    
public: // methods

    Scope( Scope * upper = 0 );
    
    virtual ~Scope();

    bool existsFunction( const std::string& name ) const;

    Function& function( const std::string& name ) const;
    
    void function( const std::string& name, Function* f );
    
private: // members
    
    typedef std::map< std::string, Function* > FunctionStore;
    
    FunctionStore functions_; // owns functions

    Scope* upper_; // not owned
    
};

//-----------------------------------------------------------------------------

} // namespace config

#endif
