/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "JoinStringsHandler.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/prelude/PrintHandler.h"

using namespace std;
using namespace eckit;

namespace eckit {

JoinStringsHandler::JoinStringsHandler(const std::string& name) : RequestHandler(name) {}

// join_strings, _ = <list_of_strings> [, separator = <separator>]
Values JoinStringsHandler::handle(ExecutionContext& context)
{
    string separator;
    if (context.environment().lookupNoThrow("separator"))
    {
        vector<string> separators (getValueAsList(context, "separator"));
        if (separators.size() > 1)
            throw UserError("join_strings: separator must be a single string");

        if (separators.size() == 1)
            separator = separators[0];
    }

    List r;
    stringstream ss;

    Values vs (context.environment().lookup("_"));
    PrintHandler::printList(ss, vs, separator, "");
    delete vs;

    r.append(ss.str());
    return r;
}

} // namespace eckit
