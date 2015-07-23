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

#include "DefineFunctionHandler.h"

using namespace std;
using namespace eckit;

namespace odb {
namespace tool {

DefineFunctionHandler::DefineFunctionHandler(const string& name)
: SpecialFormHandler(name)
{}

Request DefineFunctionHandler::handle(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "function");
    Request r (request->rest());

    Request params (0);
    if (r && r->text() == "of")
    {
        params = r->value(); 
        r = r->rest();
    }

    ASSERT(r->tag() == "" && r->text().size());
    string name (r->text());
    Request code (r->value()->value());
    ASSERT(code->tag() == "_requests");

    Log::debug() << "Defining function " << name << "(" << params << "): " << code << endl; 

    Request function (new Cell("_function", name, params, code));
    Request frame (new Cell("_frame", "definition", 0, 0));
    frame->append(new Cell("", name, function, 0));

    context.pushEnvironmentFrame(frame);

    return Cell::clone(function);
}

} // namespace tool
} // namespace odb
