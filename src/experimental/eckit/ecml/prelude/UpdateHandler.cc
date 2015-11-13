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

#include "UpdateHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {

UpdateHandler::UpdateHandler(const string& name)
: SpecialFormHandler(name)
{}

// update, _ = (let, x = 1, y = 2), x = 10  =>  let, x = 10, y = 2

Request UpdateHandler::handle(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "update");

    Request evaluatedAttributes (context.interpreter().evalAttributes(request, context));

    Cell* value (evaluatedAttributes->rest());
    if (value->text() != "_") 
        throw UserError("update: first parameter must be _ (a closure)");
    value = value->value(); 
    value = value->value();

    evaluatedAttributes = evaluatedAttributes->rest();

    Request newValue (Cell::clone(value));
    for (Request e(evaluatedAttributes->rest()); e; e = e->rest())
    {
        ASSERT(e->tag() == "");

        newValue->update(e->text(), Cell::clone(e->value()));
    }

    return newValue;
}

} // namespace eckit

