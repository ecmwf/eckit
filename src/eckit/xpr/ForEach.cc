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
    ExpPtr range = args(0, ctx, false);

    ExpPtr func  = args(1, ctx, false);

    List::value_t res;

    XprIterator it( range );

    /// XprIterator evaluates to the current entry ==> list[i]->resolve(ctx)->eval(ctx);
    ///             has a next() method (also returns the evaluated current entry)
    ///             has a bool operator to check for end

    while( it.next() )
    {
        res.push_back( func->eval( it ) );
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
