/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/ZipWith.h"
#include "eckit/maths/List.h"

//#include <algorithm>

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

ZipWith::ZipWith( ExpPtr f, ExpPtr list0, ExpPtr list1 ) : Function()
{
    push_back(f);
    push_back(list0);
    push_back(list1);
}

std::string ZipWith::returnSignature() const
{
    return List::sig();
}

ExpPtr ZipWith::evaluate( Scope &ctx ) const
{
    ExpPtr f = args(0, ctx, false);

    const List::value_t& l0 = List::extract( ctx, args(1, ctx, true) );
    const List::value_t& l1 = List::extract( ctx, args(2, ctx, true) );

    const size_t nlist = std::min(l0.size(), l1.size());

    List::value_t res;
    res.reserve(nlist);


    for( size_t i = 0; i < nlist; ++i )
    {
        ExpPtr e0 = l0[i]->eval(ctx);
        ExpPtr e1 = l1[i]->eval(ctx);

        ExpPtr v = f->eval( e0, e1 );

        res.push_back( v );
    }

     return ExpPtr(new List( res, List::Swap()));
}

void ZipWith::asCode(std::ostream&o) const
{
    o << "maths::zipWith("; printArgs(o); o <<")";
}

ExpPtr ZipWith::cloneWith(args_t &a) const {
    NOTIMP; // Should not be called
}

// Support for count()
bool ZipWith::countable() const
{
    return args(1)->countable() && args(2)->countable();
}

size_t ZipWith::count() const
{
    return std::min( args(1)->count(),  args(2)->count());
}

//--------------------------------------------------------------------------------------------

ExpPtr zipWith( ExpPtr f, ExpPtr l0, ExpPtr l1 )
{
    return ExpPtr( new ZipWith(f,l0,l1) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
