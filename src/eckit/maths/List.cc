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

List::List( const ListPtr& l ) : Value( l->args_ )
{
}

List::List( const value_t& v ) : Value(v)
{
}

void List::asCode(std::ostream&o) const
{
    o << "maths::list("; printArgs(o); o <<")";
}

bool List::is(const ExpPtr &e) {
    return dynamic_cast<List*>(e.get()) != 0;
}


ExpPtr List::clone()
{
    ListPtr l = maths::list();
    for( size_t i = 0; i < args_.size(); ++i )
    {
        l->append( args_[i]->clone() );
    }
}

ListPtr List::append(ExpPtr e)
{
    //FIXME: Not functional()

    args_.push_back(e);
    return this->as<List>();
}

ListPtr List::append(const List& l)
{
    //FIXME: Not functional()
    const value_t& t = l.args_;
    args_.reserve(args_.size() + t.size());
    std::copy(t.begin(), t.end(), std::back_inserter(args_) );
    return this->as<List>();
}

//ListPtr List::operator, ( ExpPtr e)
//{
//    DBGX(*e);
//    return append(e);
//}

//--------------------------------------------------------------------------------------------

void List::print(std::ostream&o) const
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

ListPtr list()
{
    return ListPtr( new List() );
}

ListPtr list( const List::value_t& v  )
{
    return ListPtr( new List(v) );
}

ListPtr list(ExpPtr e)
{
    return ListPtr( new List(e) );
}

ListPtr list(ExpPtr v0, ExpPtr v1)
{
    return list(v0)->append(v1);
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit
