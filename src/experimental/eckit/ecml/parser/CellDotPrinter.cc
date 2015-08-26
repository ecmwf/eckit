/*
 * (C) Copyright 1996-2013 ECMWF.
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

ostream& CellDotPrinter::printDot(ostream& s, const Cell* p, bool detailed, bool clever)
{
    if (clever)
    {
        if (p->tag() == "_list" || p->tag() == "_requests") return printDotList(s, p, detailed);
        if (p->tag() == "_verb") return printDotVerb(s, p, detailed);
    }

    s << "\"node" << (void*) p << "\" [ label=\"<f0>";
    if (detailed) s << (void*) p;
    s << " " << CellPrinter::quotedSnippet(p->text()) << " | <f1>value_ | <f2>rest_\" shape = \"record\" ];" << endl;
    if (p->value())
    {
        s << "\"node" << (void*) p << "\":f1 -> \"node" << (void*) p->value() << "\":f0;" << endl;
        printDot(s, p->value(), detailed, clever);
    }
    if (p->rest())
    {
        s << "\"node" << (void*) p << "\":f2 -> \"node" << (void*) p->rest() << "\":f0;" << endl;
        printDot(s, p->rest(), detailed, clever);
    }
    return s;
}

bool oneElementList(const Cell* p)
{
    return p->tag() == "_list"
         && p->value() 
         && ! p->rest()
         && ! (p->value()->tag().size())
           ;
}

bool textElement(const Cell* p)
{
    return p
        && p->text().size()
        //&& ! p->tag().size()
        && ! p->value()
        && ! p->rest()
        ;
}

ostream& CellDotPrinter::printDotVerb(ostream& s, const Cell* c, bool detailed)
{
    ASSERT(c->tag() == "_verb");

    stringstream box, arrows;

    box << "\"node" << "" << (void*) c << "\" [ label=\"<f0>";
    if (detailed)
        box << "\\\"" << (void*) c << "\\\"";
    box << " " << CellPrinter::quotedSnippet(c->text()) << ",";

    size_t i(1);
    for (const Cell* p(c->rest()); p; ++i, p = p->rest())
    {
        box << " | <f" << i << "> ";
        if (detailed)
            box << "\\\"" << (void*) p << "\\\"";

        box << " " << p->text() << " = "; 
        if (oneElementList(p->value()))
            box << CellPrinter::quotedSnippet(p->value()->value()->text());
        else
        {
            arrows << "\"node" << (void*) c << "\":f" << i << " -> \"node" << (void*) p->value() << "\":f0;" << endl;
            printDot(s, p->value(), detailed, true);
        }

    }
    box << "\" shape=\"record\" ]; ";
    return s << box.str() << endl << arrows.str();
}

ostream& CellDotPrinter::printDotList(ostream& s, const Cell* c, bool detailed) 
{
    ASSERT(c->tag() == "_list" || c->tag() == "_requests");

    stringstream box, arrows;

    box << "\"node" << (void*) c << "\" [ label=\"<f0> [" << c->tag() << "] ";

    size_t i(0);
    for (const Cell* p(c); p; ++i, p = p->rest())
    {
        ASSERT(p->tag() == "_list" || p->tag() == "_requests");

        if (i) 
            box << " | <f" << i << "> ";
 
        if (detailed)
            box << (void*) p;

        if (textElement(p->value()))
            box << "  " << CellPrinter::quotedSnippet(p->value()->text());
        else
        {
            arrows << "\"node" << (void*) c << "\":f" << i << " -> \"node" << (void*) p->value() << "\":f0;" << endl;
            if (p->value())
                printDot(s, p->value(), detailed, true);
        }

    }
    box << "\" shape=\"record\" ]; ";
    return s << box.str() << endl << arrows.str();
}

ostream& CellDotPrinter::dot(ostream& s, const Cell* p, const string& label, bool detailed, bool clever) 
{
    s << "digraph g  {\n"
        "graph [ rankdir = \"LR\" label=\"" << CellPrinter::quotedSnippet(label) << "\"];\n"
        "node [ fontsize = \"16\" shape = \"ellipse\" ];\n"
        "edge [ ];\n";
    printDot(s, p, detailed, clever);
    s << "}\n";
    return s;
}

} // namespace eckit

