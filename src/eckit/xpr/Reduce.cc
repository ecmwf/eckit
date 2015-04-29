/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Reduce.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Vector.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Reduce::Reduce( ExpPtr f,  ExpPtr list ) : Function()
{
    push_back(f);
    push_back(list);
}

Reduce::Reduce( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

Reduce::Reduce(Stream &s) : Function(s) {}

ExpPtr Reduce::evaluate( Scope &ctx ) const
{
    ExpPtr f = args(0, ctx, false);
    ExpPtr l = args(1, ctx, true);

    if (Vector::is(l))
    {
        const Vector::value_t& vec = Vector::extract( l );
        const size_t nvec = vec.size();
        if(!nvec) // empty vector
            return ExpPtr( new Vector(0) );

        ExpPtr e( new Real(vec[0]) );

        for( size_t i = 1; i < nvec; ++i )
        {
            e = f->eval( e, ExpPtr( new Real(vec[i]) ) );
        }

        return e->eval(ctx);
    }
    else // List
    {
        const List::value_t& list = List::extract( ctx, l );
        const size_t nlist = list.size();
        if(!nlist) // empty list
            return ExpPtr( new List() );

        ExpPtr e = list[0]->eval(ctx);

        for( size_t i = 1; i < nlist; ++i )
        {
            e = f->eval( e, list[i]->eval(ctx) );
        }

        return e->eval(ctx);
    }
}

ExpPtr Reduce::cloneWith(args_t& a) const
{
    return ExpPtr( new Reduce(a) );
}

//--------------------------------------------------------------------------------------------

ClassSpec Reduce::classSpec_ = {
    &Function::classSpec(),
    Reduce::nodeName(),
};

Reanimator< Reduce > Reduce::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr reduce( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new Reduce(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
