/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ThrowHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"

#include <vector>
#include "eckit/parser/StringTools.h"

using namespace std;

namespace eckit {

ThrowHandler::ThrowHandler(const std::string& name) : RequestHandler(name) {}

Values ThrowHandler::handle(ExecutionContext& context)
{
    vector<string> what (getValueAsList(context, "what"));

    string message;

    if (what.size())
        message = StringTools::join("/", what);
    else
    {
        const string currentException (context.environment().lookup("current_exception", "", context));
        if (currentException.size())
            message = currentException;
    }

    throw Exception(message);
    return 0;
}

} // namespace eckit
