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

#include "experimental/eckit/ecml/parser/Request.h"
#include "SpecialFormHandler.h"
#include "RequestHandler.h"
#include "Interpreter.h"
#include "ExecutionContext.h"
#include "Environment.h"

using namespace std;

namespace eckit {

Interpreter::~Interpreter() {}

Values Interpreter::evalList(const Request requests, ExecutionContext& context)
{
    Values r(0);
    List list(r);

    if (requests && requests->tag() == "_list" && requests->value() == 0 && requests->rest() == 0)
        return Cell::clone(requests);

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
        delete r;

        r = eval(request, context);
    }
    return r;
}

Values Interpreter::evalNative(const Request object, const Request request, ExecutionContext& context)
{
    RequestHandler& handler (RequestHandler::handler(object->text()));

    Request evaluatedAttributes (evalAttributes(request, context));
    context.pushEnvironmentFrame(evaluatedAttributes);

    Values r (handler.handle(context));

    context.popEnvironmentFrame(evaluatedAttributes);
    return r;
}

Values Interpreter::evalMacro(const Request object, const Request request, ExecutionContext& context)
{
    SpecialFormHandler& h (SpecialFormHandler::handler(object->text()));
    return h.handle(request, context);
}

Values Interpreter::evalVerb(const Request object, const Request request, ExecutionContext& context)
{
    Request evaluatedAttributes (evalAttributes(request, context));

    context.pushEnvironmentFrame(evaluatedAttributes);
    Cell *r (eval(object, context));
    context.popEnvironmentFrame(evaluatedAttributes);
    return r;
}

Values Interpreter::eval(const Request request, ExecutionContext& context)
{
    if (request->tag() == "") return request;
    if (request->tag() == "_requests") return evalRequests(request, context);
    if (request->tag() == "_list") return evalList(request, context);

    ASSERT((request->tag() == "_verb" || request->tag() == "_macro") && request->text().size());

    string verb (request->text());
    Request object (context.environment().lookup(verb));
    string tag (object->tag());

    if (tag == "_list")
    {
        // I know...
        string fn (object->value()->value()->text());
        // delete object;
        object = context.environment().lookup(fn);
        tag = "_verb";
    }

    Values r ( tag == ""          ? object
             : tag == "_native"   ? evalNative(object, request, context)
             : tag == "_verb"     ? evalVerb(object, request, context)
             : tag == "_macro"    ? evalMacro(object, request, context)
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
