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

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/Interpreter.h"
#include "eckit/ecml/core/SpecialFormHandler.h"

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
    if (! condition)
        throw UserError("No 'condition' passed to 'if'");

    Request then (request->valueOrDefault("then", 0));
    Request _else (request->valueOrDefault("else", 0));
    if (!then && !_else)
        throw UserError("Either 'then' or 'else' must be passed to 'if'");

    Values test (context.interpreter().eval(condition, context));
    Request r(0);

    if (! test
        || (test->tag() == "_list" && !test->value())) // empty list
    {
        if (_else) r = _else;
    }
    else
    {
        if (then) r = then;
    }

    if (r)
    {
        r = context.interpreter().eval(r, context);
        return Cell::clone(r);
    }

    return new Cell("_list", "", 0, 0);
}

} // namespace eckit

