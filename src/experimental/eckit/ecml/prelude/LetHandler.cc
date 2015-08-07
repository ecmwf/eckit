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

#include "LetHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {

LetHandler::LetHandler(const string& name)
: SpecialFormHandler(name)
{}

Request LetHandler::handle(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "let");

    Request evaluatedAttributes (context.interpreter().evalAttributes(request, context));

    Request frame (new Cell("_verb", "let", 0, 0));
    for (Request e(evaluatedAttributes->rest()); e; e = e->rest())
    {
        ASSERT(e->tag() == "");
        frame->append(new Cell("", e->text(), e->value(), 0));
    }
    context.pushEnvironmentFrame(frame);
    return Cell::clone(frame);
}

} // namespace eckit

