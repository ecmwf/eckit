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

#include "FunctionDefinition.h"

using namespace std;

namespace eckit {

// E.g.: function,of=x,capture=a/b,f=(println,values=FOO)
FunctionDefinition::FunctionDefinition(Cell* request)
{
    ASSERT(request->tag() == "_verb" && request->text() == "function");

    Request r (request->rest());

    Request capturedVariables (0);

    if (r && r->text() == "of")
    {
        parameters_ = r->valueAsListOfStrings(); 
        r = r->rest();
    }

    if (r && r->text() == "capture")
    {
        capturedVariables_ = r->valueAsListOfStrings();
        r = r->rest();
    }

    ASSERT(r && r->tag() == "" && r->text().size());
    name_ = r->text();
    code_ = r->value()->value();
    ASSERT(code_->tag() == "_requests");
}

std::string FunctionDefinition::name() const { return name_; }
std::vector<std::string> FunctionDefinition::parameters() const { return parameters_; }
std::vector<std::string> FunctionDefinition::capturedVariables() const { return capturedVariables_; }
Cell* FunctionDefinition::code() const { return code_; }

} // namespace eckit
