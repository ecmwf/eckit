/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/StringTools.h"
#include "eckit/io/FileHandle.h"
#include "eckit/parser/StringTools.h"
#include "Cell.h"
#include "CellPrinter.h"
#include "CellDotPrinter.h"

using namespace std;
using namespace eckit;

namespace eckit {

typedef StringTools S;

Cell::Cell(const std::string& tag, const std::string& text, Cell* value, Cell* rest)
: tag_(tag),
  text_(text),
  value_(value),
  rest_(rest)
{}

Cell::Cell(const Cell* o)
: tag_(o->tag_),
  text_(o->text_),
  value_(o->value_ ? new Cell(o->value_) : 0),
  rest_(o->rest_ ? new Cell(o->rest_) : 0)
{}

Cell* Cell::clone(const Cell* c) { return c ? new Cell(c) : 0; }

const std::string& Cell::tag() const { return tag_; }
const std::string& Cell::text() const { return text_; }
Cell* Cell::value() const { return value_; }
Cell* Cell::rest() const { return rest_; }

Cell* Cell::value(Cell* v) { return value_ = v; }
Cell* Cell::rest(Cell* r) { return rest_ = r; }

Cell* Cell::text(const std::string& s)
{
    text_ = s;
    return this;
}

Cell* Cell::append(Cell* r)
{ 
    Cell* last (this);
    for ( ; last->rest_; last = last->rest_)
        ;
    last->rest_ = r;
    return this;
}

Cell* Cell::value(const std::string& keyword, const std::string& value)
{
   return this->value(keyword, new Cell("_list", "", new Cell("", value, 0, 0), 0));
}

Cell* Cell::value(const std::string& keyword, Cell* v) 
{
    //showGraph(string("value: request = ") + str() + ", keyword = " + keyword + ", v = " + v->str());

    std::string k (StringTools::lower(keyword));
    for (Cell* r (this); r; r = r->rest())
    {
        if (StringTools::lower(r->text()) == k)
            return r->value(v);

        if (! r->rest())
            return r->rest(new Cell("", keyword, v, 0))->value();
    }

    ASSERT("Should not reach here" && 0);
    return 0;
}

Cell* Cell::valueOrDefault(const string& keyword, Cell* defaultValue) const
{
    //showGraph(string("valueOrDefault: request = ") + str() + ", keyword = " + keyword);
    std::string k (StringTools::lower(keyword));
    const Cell* p(0);
    for (const Cell* r (this); r; r = r->rest())
        if (StringTools::lower(r->text()) == k)
            p = r;

    if (p == 0)
        return defaultValue;

    return p->value();
}

std::string Cell::valueAsString(const string& keyword, const std::string& defaultValue) const
{
    //showGraph(string("valueAsString: request = ") + str() + ", keyword = " + keyword);

    Cell* p (valueOrDefault(keyword, 0));

    //showGraph(string("getValueAsString: found ") + p->str());

    if (! p)
        return defaultValue;

    ASSERT(p->tag() == "_list");
    string r (p->value()->text());

    if (p->rest())
        throw UserError(string("Expected only one value of ") + keyword + ", found: " + p->value()->str());

    return r;
}

std::ostream& Cell::print(std::ostream& s) const
{
    return CellPrinter::print(s, this, 0);
}

ostream& Cell::dot(ostream& s, const string& label, bool detailed, bool clever) const
{
    return CellDotPrinter::dot(s, this, label, detailed, clever);
}

std::string Cell::str() const
{
    stringstream ss;
    ss << this;
    return ss.str();
}

void Cell::graph(const string& title) { return showGraph(title, true, true, /*clever*/ false); }
void Cell::graph() { return showGraph(true, true, true); }
void Cell::simpleGraph(const string& title) { return showGraph(title, true, true, false); }
void Cell::simpleGraph() { return showGraph(true, true, false); }

void Cell::showGraph(bool background, bool detailed, bool clever)
{
    showGraph(this->str(), background, detailed, clever);
}

void Cell::showGraph(const string& label, bool background, bool detailed, bool clever)
{
    stringstream ss;
    dot(ss, label, detailed, clever);
    string s(ss.str());
    FileHandle f("graph.gv");
    f.openForWrite(s.size());
    f.write(s.c_str(), s.size());
    
    stringstream cmd;
    cmd << "(dot -Tpng -o graph.png graph.gv && xv graph.png) ";
    if (background)
        cmd << "&";
    system(cmd.str().c_str());
}

std::vector<std::string> Cell::valueAsListOfStrings() const
{
    std::vector<std::string> r;

    for (const Cell* p(this->value()); p; p = p->rest())
    {
        // TODO: check it's just a string
        if (p->value())
            r.push_back(p->value()->text());
    }

    return r;
}

} // namespace eckit

