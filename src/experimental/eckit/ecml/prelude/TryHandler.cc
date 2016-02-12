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

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/Interpreter.h"
#include "eckit/ecml/core/SpecialFormHandler.h"

#include "TryHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {


TryHandler::TryHandler(const string& name)
: SpecialFormHandler(name)
{}

/// try,
//       do = (throw,what=problem), 
//       catch = (println,values=exception caught)
//       finally = (println,values=bye)
Request TryHandler::handle(const Request request, ExecutionContext& context)
{
    auto_ptr<Cell> body (request->valueOrDefault("do", 0));
    auto_ptr<Cell> _catch (request->valueOrDefault("catch", 0));
    auto_ptr<Cell> finally (request->valueOrDefault("finally", 0));

    if (! body.get()) throw UserError("No 'do' passed to 'try'");

    bool exceptionThrown (false);
    string what;

    auto_ptr<Cell> value (0);
    if (! _catch.get() && ! finally.get())
        value.reset(context.interpreter().eval(body.get(), context));
    else
    try 
    {
        value.reset(context.interpreter().eval(body.get(), context));
        if (finally.get()) 
            context.interpreter().eval(finally.get(), context);

    } catch (eckit::Exception e) 
    {
        exceptionThrown = true;
        what = e.what(); // TODO: save type of the exception as a prefix
    }

    if (exceptionThrown)
    {
        List w;
        w.append(what);
        context.pushEnvironmentFrame(new Cell("_verb", "let", 0, new Cell("", "current_exception", w, 0)));

        if (_catch.get()) 
        {
            try {
                value.reset(context.interpreter().eval(_catch.get(), context));
            } catch (eckit::Exception ec)
            {
                if (finally.get()) 
                    context.interpreter().eval(finally.get(), context);
                throw ec;
            } 
            if (finally.get()) 
                context.interpreter().eval(finally.get(), context);
        } else if (finally.get()) 
        {
            context.interpreter().eval(finally.get(), context);
            throw eckit::Exception(what); // TODO: add info about type of exception
        }
    }
    
    if (! value.get())
        return new Cell("_list", "", 0, 0);
    return value.release();
}

} // namespace eckit

