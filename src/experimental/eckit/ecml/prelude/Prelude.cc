/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Prelude.cc
// Piotr Kuchta - (c) ECMWF July 2015

#include <string>

#include "experimental/eckit/ecml/parser/Request.h"

#include "experimental/eckit/ecml/core/RequestHandler.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/core/SpecialFormHandler.h"

#include "ListHandler.h"
#include "SequenceHandler.h"
#include "VariableLookupHandler.h"
#include "PrintHandler.h"
#include "LetHandler.h"
#include "UpdateHandler.h"
#include "DefineFunctionHandler.h"
#include "ClosureHandler.h"
#include "TestHandler.h"
#include "FirstHandler.h"
#include "RestHandler.h"
#include "IfHandler.h"
#include "TemporaryFileHandler.h"
#include "SystemHandler.h"
#include "GetenvHandler.h"
#include "ApplyHandler.h"
#include "Prelude.h"
#include "JoinStringsHandler.h"
#include "QuoteHandler.h"
#include "NullHandler.h"
#include "RunHandler.h"
#include "REPLHandler.h"
#include "DictionaryHandler.h"
#include "RangeHandler.h"
#include "ForHandler.h"
#include "GlobHandler.h"
#include "ReadTextFileHandler.h"

namespace eckit {

using namespace std;

Prelude::Prelude() {}
Prelude::~Prelude() {}

static Request native(const string& name) { return new Cell("_native", name, 0, 0); }
static Request macro(const string& name) { return new Cell("_macro", name, 0, 0); }

void Prelude::importInto(ExecutionContext& context)
{
    static DefineFunctionHandler function("function");
    static ClosureHandler closure("closure");
    static ListHandler list("list");
    static SequenceHandler sequence("sequence");
    static PrintHandler println("println", "\n");
    static PrintHandler print("print", "");
    static LetHandler let("let");
    static UpdateHandler update("update");
    static ApplyHandler apply("apply");
    static VariableLookupHandler value("value", "of");
    static VariableLookupHandler dollar("$", "_");
    static TestHandler test("test");
    static FirstHandler first("first");
    static RestHandler rest("rest");
    static TemporaryFileHandler temporary_file("temporary_file");
    static IfHandler _if("if");
    static SystemHandler _system("system");
    static GetenvHandler _getenv("getenv");
    static JoinStringsHandler join_strings("join_strings");
    static QuoteHandler quote("quote");
    static NullHandler null("null");
    static RunHandler run("run");
    static REPLHandler repl("repl");
    static DictionaryHandler dictionary("dictionary");
    static RangeHandler range("range");
    static ForHandler _for("for");
    static GlobHandler _glob("glob");
    static ReadTextFileHandler read_text_file("read_text_file");

    Environment& e(context.environment());
    e.set("let", macro(let.name()));
    e.set("update", macro(update.name()));
    e.set("function", macro(function.name()));
    e.set("apply", macro(apply.name()));
    e.set("closure", macro(closure.name()));
    e.set("test", macro(test.name()));
    e.set("if", macro(_if.name()));
    e.set("quote", macro(quote.name()));
    e.set("run", macro(run.name()));
    e.set("repl", macro(repl.name()));
    e.set("for", macro(_for.name()));

    context.registerHandler("list", list);
    context.registerHandler("sequence", sequence);
    context.registerHandler("value", value);
    context.registerHandler("$", dollar);
    context.registerHandler("print", print);
    context.registerHandler("println", println);
    context.registerHandler("first", first);
    context.registerHandler("rest", rest);
    context.registerHandler("temporary_file", temporary_file);
    context.registerHandler("system", _system);
    context.registerHandler("getenv", _getenv);
    context.registerHandler("join_strings", join_strings);
    context.registerHandler("null", null);
    context.registerHandler("dictionary", dictionary);
    context.registerHandler("range", range);
    context.registerHandler("glob", _glob);
    context.registerHandler("read_text_file", read_text_file);
    context.execute("function, of = f / values, map = ( \n"
                    "    if, condition = (first, of = ($,_ = values)), \n"
                    "        then = (sequence, values = (f, values = (first, of = ($,_ = values)))\n"
                    "                                 / (map, f = ($,_ = f),\n"
                    "                                         values = (rest, of = ($,_ = values)))))\n");
}

} // namespace eckit
