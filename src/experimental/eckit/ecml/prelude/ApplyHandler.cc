/*
 * (C) Copyright 1996-2015 ECMWF.
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
#include "eckit/ecml/ast/Closure.h"

#include "ApplyHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {

ApplyHandler::ApplyHandler(const string& name)
: SpecialFormHandler(name)
{}

Request ApplyHandler::handle(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "apply");

    Request evaluatedAttributes (context.interpreter().evalAttributes(request, context));

    Cell* p (evaluatedAttributes->rest());

    if (! p)
        throw UserError("apply needs at least one parameter; 'function' or 'closure'");

    if (p->text() != "function" && p->text() != "closure")
        throw UserError("First parameter of apply should be 'function' or 'closure'");

    Cell* closure (p->value()); // it should be a list with one closure

    if (! p->rest() || p->rest()->text() != "args" || !p->rest()->value())
        throw UserError("apply: parameter 'args' is required");

    Request paramsFrame (p->rest()->value()->value());
    ASSERT(closure->rest() == 0);

    Cell* r (0);

    context.pushEnvironmentFrame(paramsFrame);

    if (closure->value()->tag() == "_verb" && closure->value()->text() == "closure")
    {
        r = context.interpreter().evalVerb(closure->value(), paramsFrame, context);
    }
    else if (closure->value()->tag() == "_native")
        r = context.interpreter().evalNative(closure->value(), paramsFrame, context);
    else
        r = context.interpreter().eval(closure->value(), context);

    context.popEnvironmentFrame(paramsFrame);

    return r;
}

} // namespace eckit

