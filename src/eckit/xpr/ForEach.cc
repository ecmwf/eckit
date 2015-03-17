/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/ForEach.h"
#include "eckit/xpr/List.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

ForEach::ForEach( ExpPtr range,  ExpPtr func ) : Function()
{
    push_back(range);
    push_back(func);
}

ForEach::ForEach( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

ForEach::ForEach(Stream &s) : Function(s) {}

ExpPtr ForEach::evaluate( Scope &ctx ) const
{
    ExpPtr f = args(0, ctx, false);

    ExpPtr ls = args(1, ctx, true);

    const List::value_t& list = List::extract( ctx, ls );

    const size_t nlist = list.size();

    List::value_t res;
    res.reserve(nlist);

    for( size_t i = 0; i < nlist; ++i )
    {
        ExpPtr e = list[i]->resolve(ctx)->eval(ctx);
        ExpPtr v = f->eval(e);
        res.push_back( v );
    }

    return ExpPtr(new List( res, List::Swap()));
}

ExpPtr ForEach::cloneWith(args_t& a) const
{
    return ExpPtr( new ForEach(a) );
}

//--------------------------------------------------------------------------------------------

ClassSpec ForEach::classSpec_ = {
    &Function::classSpec(),
    ForEach::nodeName(),
};

Reanimator< ForEach > ForEach::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr forEach( ExpPtr range,  ExpPtr func )
{
    return ExpPtr( new ForEach(range,func) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
