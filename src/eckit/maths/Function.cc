/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "eckit/maths/Function.h"
#include "eckit/maths/Value.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Function::Function() : Expression()
{
}

Function::Function(const args_t &args) : Expression(args)
{
}

Function::~Function()
{
}

Function::dispatcher_t& Function::dispatcher()
{
    static dispatcher_t d;
    return d;
}

Function::factory_t& Function::factory()
{
    static factory_t f;
    return f;
}

string Function::signature() const
{
    return signatureArguments( args_ );
}

ExpPtr Function::evaluate( Scope &ctx )
{
    DBG;

    args_t args = args_; // create temporary args

    const size_t nargs = args.size();
    for( size_t i = 0; i < nargs; ++i )
    {
        args[i] = param(i, ctx)->eval(ctx)->self();
    }

    std::string sig = signatureArguments( args );

    DBGX(sig);

    dispatcher_t& d = dispatcher();
    dispatcher_t::iterator itr = d.find( sig );
    if( itr == d.end() )
    {
        std::ostringstream msg;
        msg << "could not dispatch to function with signature: " << sig;
        throw Error( Here(), msg.str() );
    }

    return ((*itr).second)( args );
}

ExpPtr Function::optimise()
{
    DBGX( *this );
    DBGX( signature() );

    // optimise first children

    for( size_t i = 0; i < args_.size(); ++i )
    {
        args_[i] = args_[i]->optimise()->self();
    }

    // now try to optimise self
    return Optimiser::apply( shared_from_this() );
}

ExpPtr Function::clone()
{
    key_t k = typeName();
    factory_t& f = factory();
    factory_t::iterator itr = f.find( k );
    if( itr == f.end() )
    {
        std::ostringstream msg;
        msg << "could not find factory function for type: " << k;
        throw Error( Here(), msg.str() );
    }

    // clone the arguments
    args_t args;
    for( size_t i = 0; i < args_.size(); ++i )
        args.push_back( args_[i]->clone() );

    return ((*itr).second)( args );
}

void Function::print(std::ostream&o) const
{
    o << typeName() << "("; printArgs(o); o << ")";
}

string Function::signatureArguments(const args_t &args) const
{
    std::ostringstream o;
    o << typeName() << "(";
    for( size_t i = 0; i < args.size(); ++i )
    {
        if(i) o << ",";
        o << args[i]->signature();
    }
    o << ")";
    return o.str();
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
