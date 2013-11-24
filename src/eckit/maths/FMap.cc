/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/maths/FMap.h"
#include "eckit/maths/List.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

static Function::RegisterFactory< FMap > fmap_register;

FMap::FMap(const args_t& args) : Function(args)
{
}

FMap::FMap( ExpPtr f,  ExpPtr list ) : Function()
{
    args_.push_back(f);
    args_.push_back(list);
}

string FMap::returnSignature() const
{
    return List::sig();
}

ValPtr FMap::evaluate( Scope &ctx )
{
    ExpPtr f = param(0, ctx);

    List::value_t& list = List::extract( param(1, ctx) );

    const size_t nlist = list.size();

    ListPtr res ( new List() );

    for( size_t i = 0; i < nlist; ++i )
    {
        ExpPtr e = list[i]->evaluate(ctx);

        ExpPtr v = f->eval(e);

        res->append( v );
    }

    return res;
}


void FMap::asCode(ostream &o) const
{
    o << "maths::fmap("; printArgs(o); o << ")";
}


//--------------------------------------------------------------------------------------------

ExpPtr fmap( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new FMap(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
