/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Cell.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/FileHandle.h"
#include "eckit/parser/StringTools.h"

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

Cell::Cell(Cell* o)
: tag_(o->tag_),
  text_(o->text_),
  value_(o->value_ ? new Cell(o->value_) : 0),
  rest_(o->rest_ ? new Cell(o->rest_) : 0)
{}

Cell* Cell::clone(Cell* c) { return c ? new Cell(c) : 0; }

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

string snippet(const string& s, size_t maxCharacters, size_t maxLines) 
{
    stringstream ss;

    size_t i(0), nl(0);
    for ( ;  i < s.size() && i < maxCharacters && nl < maxLines; ++i)
    {
        if (s[i] == '\n')
            ++nl;
        ss << s[i];
    }

    return ss.str() + (i < s.size() ? "..." : "");
}

string quote(const string& s) 
{
    stringstream ss;

    for (size_t i(0); i < s.size(); ++i)
        switch (s[i]) {
            case '"':
                ss << "\\\"";
                break;
            case '{': 
            case '}': 
                ss << "\\" << s[i];
                break;
            case '\n':
                ss << "\\n";
                break;
            default: 
                ss << s[i];
                break;
        }

    return ss.str();
}

string quotedSnippet(const string& s) { return quote(snippet(s, 500, 3)); }

vector<string> quote(const vector<string>& v)
{
    vector<string> r;
    for (size_t i(0); i < v.size(); ++i)
        r.push_back("\"" + v[i] + "\"");
    return r;
}

std::ostream& Cell::printAttributes(std::ostream& s, size_t depth) const
{
    for (Cell* r(rest()); r; r = r->rest())
    {
        s << ", " << r->text() << " = ";
        r->printValues(s, depth + 1);
    }
    return s;
}

std::ostream& Cell::printValues(std::ostream& s, size_t depth) const
{
    bool many (false);
    for (Cell* lst(value()); lst; lst = lst->rest(), many = true)
    {
        ASSERT(lst->tag() == "_list");
        if (many)
            s << " / ";
        if (lst->value()->text().size())
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
    if (tag_ == "_requests")
    {
        value()->print(s, depth + 1);
        for (Cell* lst(rest()); lst; lst = lst->rest())
        {
            ASSERT(lst->tag() == "_requests");
            s << "  ";
            ASSERT(lst->value());
            lst->value()->print(s, depth + 1);
        }
        return s;
    } 

    if (tag_ == "_list")
    {
        if (value()) value()->print(s, depth + 1);
        else s << " NULL ";
        for (Cell* lst(rest()); lst; lst = lst->rest())
        {
            //ASSERT(lst->tag() == "_list");
            s << " / ";
            ASSERT(lst->value());
            lst->value()->print(s, depth + 1);
        }
        return s;
    } 

    if (tag_ == "_verb")
    {
        s << (depth ? "(" : "") << text_;
        printAttributes(s, depth+ 1);
        s << (depth ? ")" : "");
        return s;
    }

    return s << text();
}


std::string Cell::str() const
{
    stringstream ss;
    ss << this;
    return ss.str();
}

ostream& Cell::printDot(ostream& s, bool detailed, bool clever) const
{
    if (clever)
    {
        if (tag() == "_list" || tag() == "_requests") return printDotList(s, detailed);
        if (tag() == "_verb") return printDotVerb(s, detailed);
    }

    s << "\"node" << (void*) this << "\" [ label=\"<f0>";
    if (detailed) s << (void*) this;
    s << " " << quotedSnippet(text()) << " | <f1>value_ | <f2>rest_\" shape = \"record\" ];" << endl;
    if (value())
    {
        s << "\"node" << (void*) this << "\":f1 -> \"node" << (void*) value() << "\":f0;" << endl;
        value()->printDot(s, detailed, clever);
    }
    if (rest())
    {
        s << "\"node" << (void*) this << "\":f2 -> \"node" << (void*) rest() << "\":f0;" << endl;
        rest()->printDot(s, detailed, clever);
    }
    return s;
}

bool oneElementList(Cell* p)
{
    return p->tag() == "_list"
         && p->value() 
         && ! p->rest()
         && ! (p->value()->tag().size())
           ;
}

bool textElement(Cell* p)
{
    return p
        && p->text().size()
        //&& ! p->tag().size()
        && ! p->value()
        && ! p->rest()
        ;
}

ostream& Cell::printDotVerb(ostream& s, bool detailed) const
{
    ASSERT(tag() == "_verb");

    stringstream box, arrows;

    box << "\"node" << (void*) this << "\" [ label=\"<f0>";
    if (detailed)
        box << (void*) this;
    box << " " << quotedSnippet(text()) << ",";

    size_t i(1);
    for (const Cell* p(rest()); p; ++i, p = p->rest())
    {
        box << " | <f" << i << "> ";
        if (detailed)
            box << (void*) p;

        box << " " << p->text() << " = "; 
        if (oneElementList(p->value()))
            box << quotedSnippet(p->value()->value()->text());
        else
        {
            arrows << "\"node" << (void*) this << "\":f" << i << " -> \"node" << (void*) p->value() << "\":f0;" << endl;
            p->value()->printDot(s, detailed, true);
        }

    }
    box << "\" shape=\"record\" ]; ";
    return s << box.str() << endl << arrows.str();
}

ostream& Cell::printDotList(ostream& s, bool detailed) const
{
    ASSERT(tag() == "_list" || tag() == "_requests");

    stringstream box, arrows;

    box << "\"node" << (void*) this << "\" [ label=\"<f0> [" << tag() << "] ";

    size_t i(0);
    for (const Cell* p(this); p; ++i, p = p->rest())
    {
        ASSERT(p->tag() == "_list" || p->tag() == "_requests");

        if (i) 
            box << " | <f" << i << "> ";
 
        if (detailed)
            box << (void*) p;

        if (textElement(p->value()))
            box << "  " << quotedSnippet(p->value()->text());
        else
        {
            arrows << "\"node" << (void*) this << "\":f" << i << " -> \"node" << (void*) p->value() << "\":f0;" << endl;
            p->value()->printDot(s, detailed, true);
        }

    }
    box << "\" shape=\"record\" ]; ";
    return s << box.str() << endl << arrows.str();
}

ostream& Cell::dot(ostream& s, const string& label, bool detailed, bool clever) const
{
    s << "digraph g  {\n"
        "graph [ rankdir = \"LR\" label=\"" << quotedSnippet(label) << "\"];\n"
        "node [ fontsize = \"16\" shape = \"ellipse\" ];\n"
        "edge [ ];\n";
    printDot(s, detailed, clever);
    s << "}\n";
    return s;
}

void Cell::graph() { return showGraph(true, true, true); }
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

