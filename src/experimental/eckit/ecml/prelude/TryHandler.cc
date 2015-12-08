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

#include "TryHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {


TryHandler::TryHandler(const string& name)
: SpecialFormHandler(name)
{}

/// try,
//       do = (do_sth,foo=bar), 
//       catch = (println,values=exception caught)
//       finally = (println,values=bye)
Request TryHandler::handle(const Request request, ExecutionContext& context)
{
    Request body (request->valueOrDefault("do", 0));
    Request _catch (request->valueOrDefault("catch", 0));
    Request finally (request->valueOrDefault("finally", 0));

    if (! body) throw UserError("No 'do' passed to 'try'");

    Values value (0);
    try {
        value = context.interpreter().eval(body, context);

        if (finally) context.interpreter().eval(finally, context);

    } catch (eckit::Exception e) 
    {
        Log::info() << "catch: '" << e.what() << "'" << endl;

        if (! _catch && ! finally)
            throw e;
        else
        {
            List w;
            w.append(e.what());
            context.pushEnvironmentFrame(new Cell("_verb", "let", 0, new Cell("", "current_exception", w, 0)));
        }

        if (_catch) 
            try {
                value = context.interpreter().eval(_catch, context);
            } catch (eckit::Exception ec)
            {
                if (finally) context.interpreter().eval(finally, context);
                throw ec;
            } 
        if (finally) 
            context.interpreter().eval(finally, context);
    }
    
    if (! value)
        return new Cell("_list", "", 0, 0);
    return value;
}

} // namespace eckit

