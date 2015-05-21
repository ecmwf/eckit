/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Request.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/FileHandle.h"
#include "eckit/parser/StringTools.h"

using namespace std;
using namespace eckit;

typedef StringTools S;

Cell::Cell(const std::string& name, Cell* value, Cell* rest)
: name_(name),
  value_(value),
  rest_(rest)
{}

Cell::Cell(Cell* o)
: name_(o->name_),
  value_(o->value_ ? new Cell(o->value_) : 0),
  rest_(o->rest_ ? new Cell(o->rest_) : 0)
{}

const std::string& Cell::name() const { return name_; }
Cell* Cell::value() const { return value_; }
Cell* Cell::rest() const { return rest_; }

Cell* Cell::value(Cell* v) { return value_ = v; }
Cell* Cell::rest(Cell* r) { return rest_ = r; }

Cell* Cell::append(Cell* r)
{ 
    Cell* last (this);
    for ( ; last->rest_; last = last->rest_)
        ;
    last->rest_ = r;
    return this;
}

vector<string> quote(const vector<string>& v)
{
    vector<string> r;
    for (size_t i(0); i < v.size(); ++i)
        r.push_back("\"" + v[i] + "\"");
    return r;
}

std::ostream& Cell::printAttributes(std::ostream& s, size_t depth) const
{
    for (Request r(rest()); r; r = r->rest())
    {
        s << ", " << r->name() << " = ";
        r->printValues(s, depth + 1);
    }
    return s;
}

std::ostream& Cell::printValues(std::ostream& s, size_t depth) const
{
    bool many (false);
    for (Cell* lst(value()); lst; lst = lst->rest(), many = true)
    {
        ASSERT(lst->name() == "_list");
        if (many)
            s << " / ";
        if (lst->value()->name().size() && lst->value()->name()[0] != '_')
        {
            s << "\"";
            lst->value()->print(s, depth + 1);
            s << "\"";
        }
        else
            lst->value()->print(s, depth + 1);
    }
    return s;
}

std::ostream& Cell::print(std::ostream& s, size_t depth) const
{
    if (! this) return s << "NULL";

    if (name_ == "_list")
    {
        s << value();
        for (Request lst(rest()); lst; lst = lst->rest())
        {
            ASSERT(lst->name() == "_list");
            s << " / ";
            ASSERT(lst->value());
            lst->value()->print(s, depth + 1);
        }
    } else
    if (name_ == "_verb")
    {
        s << (depth ? "(" : "") << value_->name();
        printAttributes(s, depth);
        s << (depth ? ")" : "");
        return s;
    }

    return s << name();
}


std::string Cell::str() const
{
    stringstream ss;
    if (this)
        ss << this;
    else
        ss << "NULL";
    return ss.str();
}

ostream& Cell::printDot(ostream& s, bool detailed) const
{
    s << "\"node" << (void*) this << "\" [ label=\"<f0>";
    if (detailed) s << (void*) this;
    s << " " << name() << " | <f1>value_ | <f2>rest_\" shape = \"record\" ];" << endl;
    if (value())
    {
        s << "\"node" << (void*) this << "\":f1 -> \"node" << (void*) value() << "\":f0;" << endl;
        value()->printDot(s);
    }
    if (rest())
    {
        s << "\"node" << (void*) this << "\":f2 -> \"node" << (void*) rest() << "\":f0;" << endl;
        rest()->printDot(s);
    }
    return s;
}

ostream& Cell::dot(ostream& s, const string& label) const
{
    s << "digraph g  {\n"
        "graph [ rankdir = \"LR\" label=\"" << S::join("\\\"", (S::split("\"", label))) << "\"];\n"
        "node [ fontsize = \"16\" shape = \"ellipse\" ];\n"
        "edge [ ];\n";
    printDot(s);
    s << "}\n";
    return s;
}

void Cell::showGraph(bool background)
{
    showGraph(this->str(), background);
}

void Cell::showGraph(const string& label, bool background)
{
    stringstream ss;
    dot(ss, label);
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
