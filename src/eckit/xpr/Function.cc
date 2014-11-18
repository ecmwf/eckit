/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Function.h"
#include "eckit/xpr/Value.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

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

/*
Function::factory_t& Function::factory()
{
    static factory_t f;
    return f;
}*/

std::string Function::signature() const
{
    return signatureArguments( args() );
}

ExpPtr Function::evaluate( Scope &ctx ) const
{
    args_t tmp = args(); // create temporary args

    const size_t nargs = tmp.size();
    for( size_t i = 0; i < nargs; ++i )
    {
        tmp[i] = args(i, ctx, true)->self();
    }

    std::string sig = signatureArguments( tmp );

    dispatcher_t& d = dispatcher();
    dispatcher_t::iterator itr = d.find( sig );
    if( itr == d.end() )
    {
        std::ostringstream msg;
        msg << "could not dispatch to function with signature: " << sig;
        throw Error( Here(), msg.str() );
    }

    return ((*itr).second)( ctx, tmp );
}

void Function::print(std::ostream&o) const
{
    o << typeName() << "("; printArgs(o); o << ")";
}

ExpPtr Function::optimise(size_t depth) const
{
    return Optimiser::apply(self(), depth);
}

std::string Function::signatureArguments(const args_t &args) const
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

} // namespace xpr
} // namespace eckit
