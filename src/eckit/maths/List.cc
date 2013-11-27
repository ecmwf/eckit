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

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

List::List() : Value()
{
}

List::List( ExpPtr e ) : Value()
{
    args_.push_back(e);
}

List::List( const value_t& v ) : Value(v)
{
}


List::List(List::value_t &v, Swap ignored )
{
    std::swap(args_, v);
}

void List::asCode(ostream &o) const
{
    o << "maths::list("; printArgs(o); o <<")";
}

bool List::is(const ExpPtr &e) {
    return dynamic_cast<List*>(e.get()) != 0;
}


ExpPtr List::clone() const
{

    const size_t nlist = args_.size();

    List::value_t res;
    res.reserve(nlist);

    for( size_t i = 0; i < args_.size(); ++i )
    {
        res.push_back(args_[i]->clone() );
    }

    return ExpPtr(new List( res, List::Swap()));
}


//--------------------------------------------------------------------------------------------

void List::print(ostream &o) const
{
    o << className() << "(";
    for( size_t i = 0; i < args_.size(); ++i )
    {
        if(i) o << ", ";
        o << *args_[i];
    }
    o << ")";
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

ExpPtr list(ExpPtr e)
{
    return ExpPtr( new List(e) );
}


//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
