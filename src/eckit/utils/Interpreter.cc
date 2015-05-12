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

Values Interpreter::eval(const std::list<Request>& requests, ExecutionContext& context)
{
    Values r;
    for(list<Request>::const_iterator j (requests.begin()); j != requests.end(); ++j)
    {
        const Request& request (*j);
        r = eval(request, context);
    }
    return r;
}

Values Interpreter::eval(const Request& request, ExecutionContext& context)
{
    RequestHandler& handler(*context.environment().lookup(requestName(request)));

    Log::info() << "Executing handler " << handler.name() << endl;

    Values r = handler.handle(request, context);

    Log::info() << "Executed " << handler.name() << ". "
                << (context.stack().size() ? "Stack:\n" : "")
                << context.stack();
    return r;
}
