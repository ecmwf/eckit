/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Merge.h"
#include "eckit/xpr/List.h"

//#include <algorithm>

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Merge::Merge( ExpPtr list0, ExpPtr list1 ) : Function()
{
    push_back(list0);
    push_back(list1);
}

Merge::Merge( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

Merge::Merge(Stream &s) : Function(s) {}

ExpPtr Merge::evaluate( Scope &ctx ) const
{
    ExpPtr f = args(0, ctx, false);

    const List::value_t& l0 = List::extract( ctx, args(0, ctx, true) );
    const List::value_t& l1 = List::extract( ctx, args(1, ctx, true) );

    List::value_t res;
    res.reserve(l0.size() + l1.size());

    for( size_t i = 0; i < l0.size(); ++i )
    {
        ExpPtr e = l0[i];
        res.push_back( e );
    }

    for( size_t i = 0; i < l1.size(); ++i )
    {
        ExpPtr e = l1[i];
        res.push_back( e );
    }

    return ExpPtr(new List( res, Expression::Swap()));
}

ExpPtr Merge::cloneWith(args_t& a) const
{
    return ExpPtr( new Merge(a) );
}

bool Merge::countable() const
{
    return args(0)->countable() && args(1)->countable();
}

size_t Merge::count() const
{
    return args(0)->count() + args(1)->count();
}

//--------------------------------------------------------------------------------------------

ClassSpec Merge::classSpec_ = {
    &Function::classSpec(),
    Merge::nodeName(),
};

Reanimator< Merge > Merge::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr merge(ExpPtr l0, ExpPtr l1 )
{
    return ExpPtr( new Merge(l0,l1) );
}

ExpPtr Merge::compute(Scope &ctx, const args_t& p )
{
    return ExpPtr( new Merge( p[0], p[1]))->eval(ctx);
}

Merge::Register::Register()
{
    Function::dispatcher()[ "xpr::Add(l,l)" ] = &compute;
}


static Merge::Register merge_register;

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
