/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "VariableLookupHandler.h"

#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/core/Interpreter.h"

using namespace std;
using namespace eckit;

namespace eckit {

VariableLookupHandler::VariableLookupHandler(const string& name) : RequestHandler(name) {}

Values VariableLookupHandler::handle(ExecutionContext& context)
{
    vector<string> vars (getValueAsList(context, "of"));
    if (vars.size() != 1)
        throw UserError("'value,of' expects one variable name currently");

    string var (vars[0]);
    Values r (context.environment().lookup(var));

    return r;
}

} // namespace eckit
