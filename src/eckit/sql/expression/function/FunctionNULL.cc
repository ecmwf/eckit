/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionNULL.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionNULL> nullFunctionBuilder("null");
static FunctionBuilder<FunctionNULL> isnullFunctionBuilder("isnull");

FunctionNULL::FunctionNULL(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionNULL::FunctionNULL(const FunctionNULL& other) :
    FunctionExpression(other.name_, other.args_) {}

FunctionNULL::~FunctionNULL() {}

std::shared_ptr<SQLExpression> FunctionNULL::clone() const {
    return std::make_shared<FunctionNULL>(*this);
}

const type::SQLType* FunctionNULL::type() const {
    return &type::SQLType::lookup("real");
}

// Don't set the missing flag
double FunctionNULL::eval(bool&) const {
    bool missing = false;
    args_[0]->eval(missing);
    return missing;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
