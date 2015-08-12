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

#include "experimental/eckit/ecml/ast/FunctionDefinition.h"
#include "experimental/eckit/ecml/ast/Closure.h"

using namespace std;

namespace eckit {

// E.g.: closure,name=f,parameters=x,captured=(environment,a=1,b=2),code=(println,values=FOO)
Closure::Closure(Cell* request)
: name_("anonymous")
{
    ASSERT(request->tag() == "_verb" && request->text() == "closure");

    Request r (request->rest());

    if (r && r->text() == "name")
    {
        vector<string> names (r->valueAsListOfStrings());
        if (names.size() > 1)
            throw UserError("Closure can only have one name");
        if (names.size())
            name_ = names[0];
        r = r->rest();
    }

    if (r && r->text() == "parameters")
    {
        parameters_ = r->valueAsListOfStrings();
        r = r->rest();
    }

    if (r && r->text() == "captured")
    {
        capturedEnvironment_ = Cell::clone(r->value());
        r = r->rest();
    } else capturedEnvironment_ = new Cell("_verb", "let", 0, 0);

    if (r && r->text() == "code")
    {
        code_ = Cell::clone(r->value());
        r = r->rest();
    }
}

Closure::Closure(const FunctionDefinition& fun, ExecutionContext& context)
: name_(fun.name()),
  parameters_(fun.parameters()),
  capturedEnvironment_(new Cell("_verb", "let", 0, 0)),
  code_(Cell::clone(fun.code()))
{
    std::vector<std::string> vs(fun.capturedVariables());
    for (size_t i(0); i < vs.size(); ++i)
    {
        capturedEnvironment_->append(new Cell("", vs[i], context.environment().lookup(vs[i]), 0));
    }
}

Closure::~Closure()
{
    //delete capturedEnvironment_;
    //delete code_;
}

// E.g.: closure,name=f,parameters=x,captured=(environment,a=1,b=2),code=(println,values=FOO)
Closure::operator Cell*() const
{
    List names;
    names.append(name_);

    List parameters;
    for (size_t i (0); i < parameters_.size(); ++i)
        parameters.append(parameters_[i]);

    List captured;
    if (capturedEnvironment_->value() || capturedEnvironment_->rest())
        captured.append(new Cell("_requests", "", Cell::clone(capturedEnvironment_), 0));

    List code;
    code.append(Cell::clone(code_));

    Cell* r (new Cell("_verb", "closure", 0,
                    new Cell("", "name", names,
                    new Cell("", "parameters", parameters,
                    new Cell("", "captured", captured,
                    new Cell("", "code", code, 0))))));

    Log::info() << "Closure::operator Cell*() => " << r << endl;
    return r;
}

Cell* Closure::capturedEnvironment() const { return capturedEnvironment_; }

Cell* Closure::code() const { return code_; }

} // namespace eckit
