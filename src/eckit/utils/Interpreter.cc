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
#include "eckit/utils/RequestHandler.h"
#include "eckit/utils/Interpreter.h"
#include "eckit/utils/ExecutionContext.h"
#include "eckit/utils/Environment.h"

using namespace std;

namespace eckit {

Values Interpreter::evalList(const Request requests, ExecutionContext& context)
{
    Values r(0);
    List list(r);

    for (Request elt(requests); elt; elt = elt->rest())
    {
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
        Log::debug() << "Evaluating request " << request << endl;
        r = eval(request, context);
    }
    return r;
}

Values Interpreter::evalNative(const Request object, const Request request, ExecutionContext& context)
{
    RequestHandler& handler (RequestHandler::handler(object->text()));
    Log::debug() << "Executing handler " << handler.name() << endl;

    Request evaluatedAttributes (evalAttributes(request, context));
    context.pushEnvironmentFrame(evaluatedAttributes);

    Values r (handler.handle(context));

    context.popEnvironmentFrame(evaluatedAttributes);

    return r;
}

Values Interpreter::evalLet(const Request request, ExecutionContext& context)
{
    ASSERT(request->tag() == "_verb" && request->text() == "let");

    Request evaluatedAttributes (evalAttributes(request, context));
    Request frame (new Cell("_frame", "let", 0, 0));
    for (Request e(evaluatedAttributes->rest()); e; e = e->rest())
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

Values Interpreter::evalFunction(const Request object, const Request request, ExecutionContext& context)
{
    ASSERT(object->tag() == "_function");
    Request body (object->rest());

    Log::debug() << "Evaluating function " << object->text() << ": " << body << endl;

    Request evaluatedRequest (evalAttributes(request, context));

    context.pushEnvironmentFrame(evaluatedRequest);
    Values r (eval(body, context));
    context.popEnvironmentFrame(evaluatedRequest);

    Log::debug() << "           function " << object->text() << " => " << r << endl;

    return r;
}

Values Interpreter::defineFunction(const Request request, ExecutionContext& context)
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

Values Interpreter::eval(const Request request, ExecutionContext& context)
{
    if (request->tag() == "_requests") return evalRequests(request, context);
    if (request->tag() == "_list") return evalList(request, context);
    if (request->tag() == "") return request;

    ASSERT("Currently we evaluate _verb only" && (request->tag() == "_verb") && request->text().size());

    const string verb (request->text());

    if (verb == "let")
        return evalLet(request, context);
    else if (verb == "function")
        return defineFunction(request, context);

    Request object (context.environment().lookup(verb));

    const string tag (object->tag());
    Values r ( tag == ""          ? object
             : tag == "_native"   ? evalNative(object, request, context)
             : tag == "_function" ? evalFunction(object, request, context)
             : tag == "_requests" ? eval(object, context)
             : tag == "_request"  ? eval(object, context)
             : 0 );

    ASSERT(r);

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
        r->append(new Cell("", name, values, 0));
    }
    return r;
}

} // namepsace eckit
