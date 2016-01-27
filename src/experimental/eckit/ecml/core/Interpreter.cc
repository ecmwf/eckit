/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/ast/Closure.h"
#include "SpecialFormHandler.h"
#include "RequestHandler.h"
#include "Interpreter.h"
#include "ExecutionContext.h"
#include "Environment.h"

using namespace std;

namespace eckit {

Interpreter::Interpreter()
: debug_(0)
{}

Interpreter::~Interpreter() {}

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
        if (object->rest() == 0)
        {
            object = object->value();
            tag = object->tag();
        } else
        {
        }
    }

    Values r ( tag == ""          ? object
             : tag == "_list"     ? object
             : tag == "_native"   ? evalNative(object, request, context)
             : tag == "_verb"     ? evalVerb(object, request, context)
             : tag == "_macro"    ? evalMacro(object, request, context)
             : tag == "_requests" ? eval(object, context)
             : tag == "_request"  ? eval(object, context)
             : 0 );
    ASSERT(r);
    return r;
}

Values Interpreter::evalList(const Request requests, ExecutionContext& context)
{
    ASSERT(requests->tag() == "_list");

    List list; 

    for (Request elt(requests); elt; elt = elt->rest())
    {
        if (! elt->tag().size())
        {
            Log::info() << "elt->tag() == " << elt->tag() <<  " " << endl;
            Log::info() << "elt == " << elt <<  " ABORTING..." << endl;
            NOTIMP;
            list.append(Cell::clone(elt->value()));
        } 
        if (! elt->value())
            continue; // empty list
        if (elt->value()->tag() == "_requests")
        {
            Values sublist (evalRequests(elt->value(), context));

            if (sublist->tag() == "_list")
            {
                for (Request e(sublist); e; e = e->rest())
                    list.append(e->value());

            } else if (sublist->tag() == "_native"
                       || sublist->tag() == "_macro"
                       || sublist->tag() == "_verb"
                       || !sublist->tag().size())
            {
                list.append(sublist);
            } else 
            {
                NOTIMP;
            }
        }
        else if (elt->value()->tag() == "_list")
        {
            Values sublist (elt->value());
            ASSERT(sublist->tag() == "_list");

            for (Request e(sublist); e; e = e->rest())
                list.append(e->value());
        } else if (elt->value()->tag() == "")
        {
            list.append(elt->value());
        } else if (elt->value()->tag() == "_verb"
                    || elt->value()->tag() == "_macro")
        {
            Values sublist (elt->value());
            list.append(sublist);
        } else {
            NOTIMP;
        }
    }

    return list;
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
    ASSERT(object->text() == "closure");

    Closure closure (object);

    Request evaluatedAttributes (evalAttributes(request, context));

    Cell* captured (closure.capturedEnvironment());
    Cell* staticEnvironment (captured ? (captured->value() ? captured->value()->value() : 0) : 0);

    context.pushEnvironmentFrame(evaluatedAttributes);
    if (staticEnvironment) context.pushEnvironmentFrame(staticEnvironment);

    Cell *r (eval(closure.code(), context));

    if (staticEnvironment) context.popEnvironmentFrame(staticEnvironment);
    context.popEnvironmentFrame(evaluatedAttributes);
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

        const string& attributeName (e->text());
        Values unevaluatedValues (e->value());

        Values values (evalList(unevaluatedValues, context));

        r->append(new Cell("", attributeName, values, 0));
    }
    return r;
}

} // namepsace eckit
