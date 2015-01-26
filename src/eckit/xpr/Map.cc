/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/xpr/Map.h"
#include "eckit/xpr/List.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

Map::Map( ExpPtr f,  ExpPtr list ) : Function()
{
    push_back(f);
    push_back(list);
}

Map::Map( args_t& a ) : Function(a)
{
    ASSERT( a.size() == 2 );
}

Map::Map(Stream &s) : Function(s) {}

void Map::encode(Stream &s) const
{
    Function::encode(s);
}

std::string Map::returnSignature() const
{
    return List::sig();
}

ExpPtr Map::evaluate( Scope &ctx ) const
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


void Map::asCode(std::ostream&o) const
{
    o << "xpr::map("; printArgs(o); o << ")";
}

ExpPtr Map::cloneWith(args_t& a) const
{
    return ExpPtr( new Map(a) );
}

bool Map::countable() const
{
    return args(1)->countable();
}

size_t Map::count() const
{
    return args(1)->count();
}

//--------------------------------------------------------------------------------------------

ClassSpec Map::classSpec_ = {
    &Function::classSpec(),
    Map::nodeName().c_str(),
};

Reanimator< Map > Map::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr map( ExpPtr f,  ExpPtr list )
{
    return ExpPtr( new Map(f,list) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
