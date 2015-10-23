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

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/core/Interpreter.h"

using namespace std;
using namespace eckit;

namespace eckit {

VariableLookupHandler::VariableLookupHandler(const string& name, const string& parameterName) 
: RequestHandler(name),
  of_(parameterName)
{}

Values VariableLookupHandler::handle(ExecutionContext& context)
{
    vector<string> vars (getValueAsList(context, of_));
    string var (vars[0]);
    Values r (context.environment().lookup(var));

    for (size_t i (1); i < vars.size(); ++i)
    {
        const string& key (vars[i]);

        // r is (possibly) a list, let's assume it has only one element for now
        // TODO: handle case where there is more elements 
        Cell * v (r->value());

        Cell * values (v->valueOrDefault(key, 0));

        if (! values)
            throw UserError(string("No '") + key + "' in " + r->str());

        delete r;
        r = values;
    }

    return Cell::clone(r);
}

} // namespace eckit
