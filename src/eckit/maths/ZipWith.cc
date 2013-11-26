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

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

ZipWith::ZipWith(const args_t& args) : Function(args)
{
    ASSERT(arity() == 3);
}

ZipWith::ZipWith( ExpPtr f, ExpPtr list0, ExpPtr list1 ) : Function()
{
    args_.push_back(f);
    args_.push_back(list0);
    args_.push_back(list1);
}

string ZipWith::returnSignature() const
{
    return List::sig();
}

ExpPtr ZipWith::evaluate( Scope &ctx )
{
    ExpPtr f = param(0, ctx);

    List::value_t& l0 = List::extract( param(1, ctx) );
    List::value_t& l1 = List::extract( param(2, ctx) );

    ASSERT( l0.size() == l1.size() );

    const size_t nlist = l0.size();

    ListPtr res ( new List() );

    for( size_t i = 0; i < nlist; ++i )
    {
        ExpPtr e0 = l0[i]->evaluate(ctx);
        ExpPtr e1 = l1[i]->evaluate(ctx);

        ExpPtr v = f->eval( e0, e1 );

        res->append( v );
    }

    return res;
}

void ZipWith::asCode(ostream &o) const
{
    o << "maths::zipWith("; printArgs(o); o <<")";
}

//--------------------------------------------------------------------------------------------

static Function::RegisterFactory< ZipWith > zipwith_register;                                                  \

//--------------------------------------------------------------------------------------------

ExpPtr zipWith( ExpPtr f, ExpPtr l0, ExpPtr l1 )
{
    return ExpPtr( new ZipWith(f,l0,l1) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
