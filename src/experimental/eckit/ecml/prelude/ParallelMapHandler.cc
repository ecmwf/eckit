/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ParallelMapHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/prelude/PrintHandler.h"
#include "experimental/eckit/ecml/ast/Closure.h"
#include "experimental/eckit/ecml/core/Interpreter.h"

using namespace std;
using namespace eckit;

namespace eckit {

ParallelMapHandler::ParallelMapHandler(const std::string& name) : RequestHandler(name) {}

Values ParallelMapHandler::handle(ExecutionContext& context)
{
    List r;

    Values values (context.environment().lookup("values"));
    Values closure (context.environment().lookup("closure"));

    for (Cell* p(values); p; p = p->rest())
    {
        Cell* v (p->value());

        // wrap it in a list
        List vs;
        vs.append(v);

        Values result (apply(context, closure, vs));
        r.append(result);
    }

    return r;
}

Values ParallelMapHandler::apply(ExecutionContext& context, Cell* closure, Cell* values)
{
    Cell* wrappedClosure (new Cell("_verb", "function", 0,
                            new Cell(
                                "",
                                "_",
                                new Cell(
                                    "_list",
                                    "",
                                    new Cell("_requests", "", Cell::clone(closure), 0),
                                    0),
                                0)));

    Request appl (new Cell("_verb", "apply", 0,
                        new Cell("", "closure", new Cell("_list", "", new Cell("_requests", "", wrappedClosure, 0), 0),
                        new Cell("", "values", Cell::clone(values), 0))));

    Request requests (new Cell("_list", "", new Cell("_requests", "", appl, 0), 0));

    return context.interpreter().eval(requests, context);
}

} // namespace eckit
