/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/config/Scope.h"
#include "eckit/config/Function.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

Scope::Scope(Scope *upper) :
    upper_(upper)
{
}

Scope::~Scope()
{
}

bool Scope::existsFunction(const string &name) const
{
    FunctionStore::const_iterator f = functions_.find(name);
    if( f == functions_.end() )
    {
        // continue search on upper scope
        if( upper_ )
            return upper_->existsFunction(name);
        else
            throw StreamParser::Error( string("Could not find function \'" + name + "\' in scope" ) );
    }
    ASSERT( f->second );
    return true;
}

Function& Scope::function(const string& name) const
{
    FunctionStore::const_iterator f = functions_.find(name);
    if( f == functions_.end() )
    {
        // continue search on upper scope
        if( upper_ )
            return upper_->function(name);
        else
            throw StreamParser::Error( string("Could not find function " + name + "in scope" ) );
    }

    ASSERT( f->second );
    return *(f->second);
}

void Scope::function( const string& name, Function* f )
{
    ASSERT( f );
    FunctionStore::iterator fi = functions_.find(name);
    if( fi != functions_.end() )
        delete fi->second;
    functions_[name] = f;
}

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit

