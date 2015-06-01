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
        //Log::info() << "Interpreter::evalList: [" << elt->tag() << "] " << elt << endl;

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

    //Log::info() << "Interpreter::evalList: => " << r << endl;

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
    Log::info() << "Executed " << handler.name() << ".";

    context.popEnvironmentFrame(request);

    //request->showGraph(string("Executed handler ") + handler.name() + ", values: " + r->str(), false);
    return r;
}

Values Interpreter::evalLet(const Request request, ExecutionContext& context)
{
    Request frame (new Cell("_verb", "let", 0, 0));
    ASSERT(request->tag() == "_verb" && request->text() == "let");
    for (Request e(request->rest()); e; e = e->rest())
    {
        ASSERT(e->tag() == "");

        const string& name (e->text());
        Values values (evalList(e->value(), context));

        frame->append(new Cell("", name, values, 0));
    }
    context.pushEnvironmentFrame(frame);
    // Not sure what to return here. The job is done with the push of new frame.
    return Cell::clone(frame);
}

Values Interpreter::eval(const Request request, ExecutionContext& context)
{
    if (request->tag() == "_requests")
        return evalRequests(request, context);

    ASSERT("Currently we evaluate _verb only" && (request->tag() == "_verb") && request->text().size());

    const string verb (request->text());
    Request evaluatedAttributes (evalAttributes(request, context));
    // TODO: release request

    if (verb == "let")
        return evalLet(evaluatedAttributes, context);

    Request object (context.environment().lookup(verb));

    const string tag (object->tag());
    Values r ( tag == "_native" ? evalNative(object, evaluatedAttributes, context)
             : tag == ""         ? object
             : tag == "_requests" ? eval(object, context)
             : tag == "_request" ? eval(object, context)
             : 0 );

    if (!r)
        NOTIMP; 

    return r;
}

Request Interpreter::evalAttributes(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb");
    ASSERT(request->value() == 0);

    Request r(new Cell(request->tag(), request->text(), 0, 0));

    for (Request e(request->rest()); e; e = e->rest())
    {
        ASSERT(e->tag() == "");
        string name (e->text());
        Values values (evalList(e->value(), context));

        Log::info() << "let: " << name << ": " << values << endl;

        r->append(new Cell("", name, values, 0));
    }
    return r;
}

