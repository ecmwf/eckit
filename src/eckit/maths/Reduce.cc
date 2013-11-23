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

static Func::RegisterFactory< Reduce > filter_register;

Reduce::Reduce(const args_t &args) : Func(args)
{
}

Reduce::Reduce( ExpPtr f,  ExpPtr list ) : Func()
{
    args_.push_back(f);
    args_.push_back(list);
}

string Reduce::ret_signature() const
{
    return Undef::sig(); /// @todo review this -- it could be the signature iterated over the list
}

ValPtr Reduce::evaluate( Context &ctx )
{
    ExpPtr f = param(0, ctx);

    List::value_t& list = List::extract( param(1, ctx) );

    const size_t nlist = list.size();

    if(!nlist) // empty list
        return ValPtr( new List() );

    ExpPtr e = list[0]->eval(ctx);

    for( size_t i = 1; i < nlist; ++i )
    {
        DBGX(*e);
        e = f->eval( e, list[i]->eval(ctx) );
    }

    return e->eval(ctx);
}

//--------------------------------------------------------------------------------------------

ExpPtr reduce( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new Reduce(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
