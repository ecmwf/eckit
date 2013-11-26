/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Reduce.h"
#include "eckit/maths/List.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

static Function::RegisterFactory< Reduce > filter_register;

Reduce::Reduce(const args_t &args) : Function(args)
{
    ASSERT(arity() == 2);
}

Reduce::Reduce( ExpPtr f,  ExpPtr list ) : Function()
{
    args_.push_back(f);
    args_.push_back(list);
}

string Reduce::returnSignature() const
{
    return Undef::sig(); /// @todo review this -- it could be the signature iterated over the list
}

ExpPtr Reduce::evaluate( Scope &ctx )
{
    ExpPtr f = param(0, ctx);

    List::value_t& list = List::extract( param(1, ctx) );

    const size_t nlist = list.size();

    if(!nlist) // empty list
        return ExpPtr( new List() );

    ExpPtr e = list[0]->evaluate(ctx);

    for( size_t i = 1; i < nlist; ++i )
    {
        DBGX(*e);
        e = f->eval( e, list[i]->evaluate(ctx) );
    }

    return e->evaluate(ctx);
}


void Reduce::asCode(std::ostream&o) const
{
    o << "maths::reduce("; printArgs(o); o <<")";
}

//--------------------------------------------------------------------------------------------

ExpPtr reduce( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new Reduce(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
