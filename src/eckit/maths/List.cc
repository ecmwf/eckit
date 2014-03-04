/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iterator>     // std::back_inserter
#include <algorithm>    // std::copy

#include "eckit/maths/List.h"
#include "eckit/maths/Optimiser.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

List::List() : Value()
{
}

List::List( ExpPtr e ) : Value()
{
    push_back(e);
}

List::List(const args_t& args ) : Value(args)
{
}


List::List(args_t &args, Swap ignored) : Value(args, ignored)
{
}

void List::asCode(std::ostream&o) const
{
    o << "maths::list("; printArgs(o); o <<")";
}

//--------------------------------------------------------------------------------------------

void List::print(std::ostream&o) const
{
    o << className() << "(";
    for( size_t i = 0; i < size(); ++i )
    {
        if(i) o << ", ";
        o << *args(i);
    }
    o << ")";
}

ExpPtr List::cloneWith(args_t& a) const
{
    return ExpPtr(new List(a, List::Swap()));
}

//--------------------------------------------------------------------------------------------

ExpPtr list()
{
    return ExpPtr( new List() );
}

ExpPtr list( const List::value_t& v  )
{
    return ExpPtr( new List(v) );
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
