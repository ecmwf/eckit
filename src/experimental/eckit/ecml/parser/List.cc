/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "List.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/FileHandle.h"
#include "eckit/parser/StringTools.h"

using namespace std;
using namespace eckit;

namespace eckit {

typedef StringTools S;

List::List(Cell*& c) : cell_(c) {}

size_t List::size() const
{
    size_t n(0);
    for (Cell* r(cell_); r; r = r->rest())
        ++n;
    return n;
}

List& List::append(Cell* c)
{
    if (cell_ == 0)
        cell_ = new Cell("_list", "", c, 0); 
    else
        cell_->append(new Cell("_list", "", c, 0));
    return *this;
}

List& List::append(const string& s)
{
    return append(new Cell("", s, 0, 0));
}

std::ostream& List::print(std::ostream& s) const
{
    return s;
}

} // namespace eckit

