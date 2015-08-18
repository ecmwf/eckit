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

    ASSERT("apply handles closures only now" && p->text() == "closure");

    Cell* closure (p->value()); // it should be a list with one closure

    Request frame (new Cell("_verb", "let", 0, 0));
    for (Request e(p->rest()); e; e = e->rest())
    {
        ASSERT(e->tag() == "");
        frame->append(new Cell("", e->text(), e->value(), 0));
    }
    context.pushEnvironmentFrame(frame);
    // closure is a list here. it should contain just one closure object.
    ASSERT(closure->rest() == 0);
    Cell *r (context.interpreter().eval(closure->value(), context));
    context.popEnvironmentFrame(frame);

    return r;
}

} // namespace eckit

