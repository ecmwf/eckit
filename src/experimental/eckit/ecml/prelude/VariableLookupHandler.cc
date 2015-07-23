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

VariableLookupHandler::VariableLookupHandler(const string& name) : RequestHandler(name) {}

Values VariableLookupHandler::handle(ExecutionContext& context)
{
    vector<string> vars (getValueAsList(context, "of"));
    ASSERT("'value,of' expects one variable name currently" && vars.size() == 1);

    string var (vars[0]);
    Values r (context.interpreter().eval(context.environment().lookup(var), context));

    Log::debug() << "value,of=" << var << " => " << r << endl;

    return r;
}

