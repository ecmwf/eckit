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

using namespace std;
using namespace eckit;

namespace eckit {

typedef StringTools S;

string CellPrinter::snippet(const string& s, size_t maxCharacters, size_t maxLines) 
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

string CellPrinter::quote(const string& s) 
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

string CellPrinter::quotedSnippet(const string& s) { return quote(snippet(s, 500, 3)); }

vector<string> CellPrinter::quote(const vector<string>& v)
{
    vector<string> r;
    for (size_t i(0); i < v.size(); ++i)
        r.push_back("\"" + v[i] + "\"");
    return r;
}

std::ostream& CellPrinter::printAttributes(std::ostream& s, const Cell* p, size_t depth) 
{
    for (Cell* r(p->rest()); r; r = r->rest())
    {
        s << ", " << r->text() << " = ";
        printValues(s, r, depth + 1);
    }
    return s;
}

std::ostream& CellPrinter::printValues(std::ostream& s, const Cell* p, size_t depth)
{
    bool many (false);
    for (Cell* lst(p->value()); lst; lst = lst->rest(), many = true)
    {
        ASSERT(lst->tag() == "_list");
        if (many)
            s << " / ";

        if (! lst->value())
            s << "(null)";
        else
        {
            //if (lst->value()->text().size())
            if (! lst->value()->tag().size())
            {
                s << "\"";
                print(s, lst->value(), depth + 1);
                s << "\"";
            }
            else
                print(s, lst->value(), depth + 1);
        }
    }
    return s;
}

std::ostream& CellPrinter::print(std::ostream& s, const Cell* p, size_t depth) 
{
    if (p->tag() == "_requests")
    {
        s << "(";
        print(s, p->value(), 0);
        for (Cell* lst(p->rest()); lst; lst = lst->rest())
        {
            ASSERT(lst->tag() == "_requests");
            s << "\n";
            ASSERT(lst->value());
            print(s, lst->value(), 0);
        }
        s << ")";
        return s;
    } 

    if (p->tag() == "_list")
    {
        if (p->value()) 
            print(s, p->value(), depth + 1);
        else
            s << "(null)";
        for (Cell* lst(p->rest()); lst; lst = lst->rest())
        {
            //ASSERT(lst->tag() == "_list");
            if (lst->value())
            {
                s << " / ";
                print(s, lst->value(), depth + 1);
            }
        }
        return s;
    } 

    if (p->tag() == "_verb")
    {
        s << (depth ? "(" : "") << p->text();
        printAttributes(s, p, depth + 1);
        s << (depth ? ")" : "");
        return s;
    }

    return s << p->text();
}

} // namespace eckit

