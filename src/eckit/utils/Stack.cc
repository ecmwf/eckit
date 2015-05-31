/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/utils/Stack.h"

using namespace std;
using namespace eckit;

Stack::Stack()
: stack_()
{}

size_t Stack::size() const
{
    return stack_.size();
}

void Stack::push(const Request r)
{
    //r->showGraph(string("Stack::push: ") + r->str(), false);

    stack_.push_back(r);
}

Cell* Stack::top()
{
    if (! stack_.size())
        throw UserError("Stack empty");

    Cell* r(stack_.back());

    //r->showGraph(string("Stack::top: =>") + r->str(), false);

    return r;
}

Cell* Stack::pop()
{
    if (! stack_.size())
        throw UserError("Stack empty");

    Cell* r (stack_.back());

    stack_.pop_back();

    //r->showGraph(string("Stack::pop: =>") + r->str(), false);

    return r;
}

std::ostream& Stack::print(std::ostream& s) const
{
    for (size_t i(0); i < stack_.size(); ++i)
        s << "[" << i << "] " << stack_[i] << endl;
    return s;
}

