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
#include "eckit/parser/StringTools.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/core/Interpreter.h"
#include "experimental/eckit/ecml/core/SpecialFormHandler.h"

#include "IfHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {

IfHandler::IfHandler(const string& name)
: SpecialFormHandler(name)
{}

/// Accepted parameters: condition, then, else
Request IfHandler::handle(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "if");

    Request condition (request->valueOrDefault("condition", 0));
    if (! condition) throw UserError("No 'condition' passed to 'if'");

    Values test (context.interpreter().eval(condition, context));
    Request r(0);
    //cout << " *** IfHandler: evaluated condition: " << (test ? test->str() : "NULL") << endl;
    if (! test
        || (test->tag() == "_list" && !test->value())) // empty list
    {
        Request _else (request->valueOrDefault("else", 0));
        if (_else) r = _else;
    }
    else
    {
        Request then (request->valueOrDefault("then", 0));
        if (then) r = then;
    }
    if (!r) throw UserError("Either 'then' or 'else' must be passed to 'if'");

    r = context.interpreter().eval(r, context);
    return Cell::clone(r);
}

} // namespace eckit

