/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/parser/Request.h"
#include "eckit/utils/Interpreter.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"

using namespace std;
using namespace eckit;

Values Interpreter::evalList(const Request requests, ExecutionContext& context)
{
    Values r(0);
    for(Request request(requests->rest()); request; request = request->rest())
    {
        delete r;
        r = eval(request, context);
    }
    return r;
}

Values Interpreter::eval(const Request request, ExecutionContext& context)
{
    if (request->name() == "_list")
        return evalList(request, context);

    ASSERT("Currently we evaluate _verb only" && (request->name() == "_verb"));
    ASSERT(request->value());

    string verb (request->value()->name());
    RequestHandler& handler(*context.environment().lookup(verb));

    Log::info() << "Executing handler " << handler.name() << endl;

    //request->showGraph(string("Executing handler ") + handler.name() + ", request: " + request->str(), true);

    Values r = handler.handle(request, context);

    Log::info() << "Executed " << handler.name() << ". "
                << (context.stack().size() ? "Stack:\n" : "")
                << context.stack();

    //request->showGraph(string("Executed handler ") + handler.name() + ", values: " + r->str(), false);

    return r;
}
