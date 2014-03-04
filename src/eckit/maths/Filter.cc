/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/Filter.h"
#include "eckit/maths/List.h"
#include "eckit/maths/Boolean.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

Filter::Filter( ExpPtr pred,  ExpPtr list ) : Function()
{
    push_back(pred);
    push_back(list);
}

Filter::Filter(args_t& a) : Function(a)
{
    ASSERT( a.size() == 2);
}

std::string Filter::returnSignature() const
{
    return List::sig();
}

ExpPtr Filter::evaluate( Scope &ctx ) const
{
    ExpPtr f = args(0, ctx, false);

    const List::value_t& list = List::extract( ctx, args(1, ctx, true) );

    const size_t nlist = list.size();

    List::value_t res;
    res.reserve(nlist);

    for( size_t i = 0; i < nlist; ++i )
    {
        ExpPtr e = list[i]->eval(ctx);
        ExpPtr b = f->eval(e);

        if( Boolean::extract(ctx, b) )
            res.push_back( e );
    }

    return ExpPtr(new List( res, List::Swap()));
}


void Filter::asCode(std::ostream&o) const
{
    o << "maths::filter("; printArgs(o); o <<")";
}


ExpPtr Filter::cloneWith(args_t& a) const
{
    return ExpPtr(new Filter(a));
}

//--------------------------------------------------------------------------------------------

ExpPtr filter( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new Filter(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
