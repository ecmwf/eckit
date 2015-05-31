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
    List list(r);

    for (Request elt(requests); elt; elt = elt->rest())
    {
        Log::info() << "Interpreter::evalList: [" << elt->tag() << "] " << elt << endl;

        if (! elt->tag().size())
            list.append(elt);
        else
        {
            Values sublist (elt->value()->tag() == "_requests"  
                            ? evalRequests(elt->value(), context)
                            : evalList(elt->value(), context));

            for (Request e(sublist); e; e = e->rest())
                list.append(e->value());
        }
    }

    Log::info() << "Interpreter::evalList: => " << r << endl;

    return r;
}

Values Interpreter::evalRequests(const Request requests, ExecutionContext& context)
{
    Values r(0);
    for (Request elt(requests); elt; elt = elt->rest())
    {
        Request request( elt->value() );
        //TODO:
        //delete r;
        Log::info() << "Evaluating request " << request << endl;
        r = eval(request, context);
    }
    return r;
}

Values Interpreter::evalNative(const Request object, const Request request, ExecutionContext& context)
{
    RequestHandler& handler (RequestHandler::handler(object->text()));
    Log::info() << "Executing handler " << handler.name() << endl;

    context.pushEnvironmentFrame(request);

    Values r (handler.handle(context));
    //request->showGraph(string("Executing handler ") + handler.name() + ", request: " + request->str(), true);
    Log::info() << "Executed " << handler.name() << ". "
                << (context.stack().size() ? "Stack:\n" : "")
                << context.stack();

    context.popEnvironmentFrame();

    //request->showGraph(string("Executed handler ") + handler.name() + ", values: " + r->str(), false);
    return r;
}

Values Interpreter::eval(const Request request, ExecutionContext& context)
{
    if (request->tag() == "_requests")
        return evalRequests(request, context);

    ASSERT("Currently we evaluate _verb only" && (request->tag() == "_verb") && request->text().size());

    string verb (request->text());
    Request object (context.environment().lookup(verb));

    Values r(0);
    if (object->tag() == "_native") r = evalNative(object, request, context);
    else if (object->tag() == "") r = object;
    else if (object->tag() == "_requests") r = eval(object, context);
    else if (object->tag() == "_request") r = eval(object, context);
    else
    {
       NOTIMP; 
    }

    return r;
}
