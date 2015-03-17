/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/While.h"
#include "eckit/xpr/List.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

While::While( ExpPtr range,  ExpPtr func ) : Function()
{
    push_back(range);
    push_back(func);
}

While::While( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

While::While(Stream &s) : Function(s) {}

ExpPtr While::evaluate( Scope &ctx ) const
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

ExpPtr While::cloneWith(args_t& a) const
{
    return ExpPtr( new While(a) );
}

//--------------------------------------------------------------------------------------------

ClassSpec While::classSpec_ = {
    &Function::classSpec(),
    While::nodeName(),
};

Reanimator< While > While::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr while( ExpPtr range,  ExpPtr func )
{
    return ExpPtr( new While(range,func) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
