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

#include "eckit/xpr/List.h"
#include "eckit/xpr/Optimiser.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

bool List::is(const ExpPtr& e)
{
    return e->typeName() == List::nodeName();
}

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

List::List(Stream &s) : Value(s) {}

void List::encode(Stream &s) const
{
    Value::encode(s);
}

//--------------------------------------------------------------------------------------------

void List::print(std::ostream&o) const
{
    o << nodeName() << "(";
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

ClassSpec List::classSpec_ = {
    &Value::classSpec(),
    List::nodeName().c_str(),
};

Reanimator< List > List::reanimator_;

//--------------------------------------------------------------------------------------------

ExpPtr list()
{
    return ExpPtr( new List() );
}

ExpPtr list( const List::value_t& v  )
{
    return ExpPtr( new List(v) );
}

ExpPtr list( ExpPtr head, ExpPtr tail )
{
    List::value_t lst;

    if( List::is(head) )
    {
        const List::value_t& h = head->as<List>()->value();
        lst.insert( lst.end(), h.begin(), h.end() );
    }
    else
        lst.push_back(head);

    if( List::is(tail) )
    {
        const List::value_t& t = tail->as<List>()->value();
        lst.insert( lst.end(), t.begin(), t.end() );
    }
    else
        lst.push_back(tail);

    return ExpPtr( new List(lst) );
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit
