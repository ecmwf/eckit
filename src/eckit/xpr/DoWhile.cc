/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/DoWhile.h"
#include "eckit/xpr/List.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

DoWhile::DoWhile( ExpPtr pred,  ExpPtr func ) : Function()
{
    push_back(pred);
    push_back(func);
}

DoWhile::DoWhile( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

DoWhile::DoWhile(Stream &s) : Function(s) {}

ExpPtr DoWhile::evaluate( Scope &ctx ) const
{
    ExpPtr pred = args(0, ctx, false);
    ExpPtr func = args(1, ctx, true);

    /// ....

    //  return ExpPtr(new List( res, List::Swap()));
}

ExpPtr DoWhile::cloneWith(args_t& a) const
{
    return ExpPtr( new DoWhile(a) );
}

//--------------------------------------------------------------------------------------------

ClassSpec DoWhile::classSpec_ = {
    &Function::classSpec(),
    DoWhile::nodeName(),
};

Reanimator< DoWhile > DoWhile::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr doWhile( ExpPtr range,  ExpPtr func )
{
    return ExpPtr( new DoWhile(range,func) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
