/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "eckit/ecml/parser/Request.h"

#include "eckit/ecml/core/RequestHandler.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/SpecialFormHandler.h"

#include "ListHandler.h"
#include "VariableLookupHandler.h"
#include "PrintHandler.h"
#include "LetHandler.h"
#include "DefineFunctionHandler.h"
#include "ClosureHandler.h"
#include "TestHandler.h"
#include "FirstHandler.h"
#include "RestHandler.h"
#include "IfHandler.h"
#include "TemporaryFileHandler.h"
#include "SystemHandler.h"
#include "GetenvHandler.h"
#include "ParallelMapHandler.h"
#include "ApplyHandler.h"
#include "Prelude.h"
#include "JoinStringsHandler.h"
#include "QuoteHandler.h"
#include "NullHandler.h"
#include "RunHandler.h"
#include "REPLHandler.h"

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
    static PrintHandler println("println", "\n");
    static PrintHandler print("print", "");
    static LetHandler let("let");
    static ApplyHandler apply("apply");
    static VariableLookupHandler value("value");
    static VariableLookupHandler dollar("$");
    static TestHandler test("test");
    static FirstHandler first("first");
    static RestHandler rest("rest");
    static TemporaryFileHandler temporary_file("temporary_file");
    static IfHandler _if("if");
    static SystemHandler _system("system");
    static GetenvHandler _getenv("getenv");
    static ParallelMapHandler _map("map");
    static JoinStringsHandler join_strings("join_strings");
    static QuoteHandler quote("quote");
    static NullHandler null("null");
    static RunHandler run("run");
    static REPLHandler repl("repl");

    Environment& e(context.environment());
    e.set("let", macro(let.name()));
    e.set("function", macro(function.name()));
    e.set("apply", macro(apply.name()));
    e.set("closure", macro(closure.name()));
    e.set("test", macro(test.name()));
    e.set("if", macro(_if.name()));
    e.set("quote", macro(quote.name()));
    e.set("run", macro(run.name()));
    e.set("repl", macro(repl.name()));

    context.registerHandler("list", list);
    context.registerHandler("value", value);
    context.registerHandler("$", dollar);
    context.registerHandler("print", print);
    context.registerHandler("println", println);
    context.registerHandler("first", first);
    context.registerHandler("rest", rest);
    context.registerHandler("temporary_file", temporary_file);
    context.registerHandler("system", _system);
    context.registerHandler("getenv", _getenv);
    context.registerHandler("map", _map);
    context.registerHandler("join_strings", join_strings);
    context.registerHandler("null", null);
}

} // namespace eckit
