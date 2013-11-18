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

#include "eckit/maths/Func.h"
#include "eckit/maths/Value.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Func::Func() : Expression()
{
}

Func::Func(const args_t &args) : Expression(args)
{
}

Func::~Func()
{
}

Func::dispatcher_t &Func::dispatcher()
{
    static dispatcher_t d;
    return d;
}

ValPtr Func::evaluate( context_t& ctx )
{
    DBG;

    args_t args = args_; // create temporary args

    const size_t nargs = args.size();
    for( size_t i = 0; i < nargs; ++i )
    {
        args[i] = param(i,&ctx)->evaluate(ctx)->self();
    }

    std::string sig = signature_args( args );

    DBGX(sig);

    dispatcher_t& d = dispatcher();
    dispatcher_t::iterator itr = d.find( sig );
    if( itr == d.end() )
    {
        ostringstream msg;
        msg << "could not dispatch to function with signature: " << sig;
        throw Error( Here(), msg.str() );
    }

    return ((*itr).second)( args );
}

ExpPtr Func::optimise()
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

void Func::print(ostream &o) const
{
    o << type_name() << "(";
    for( size_t i = 0; i < arity(); ++i )
    {
        if(i) o << ", ";
        o << *args_[i]; // no context applied
    }
    o << ")";
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
