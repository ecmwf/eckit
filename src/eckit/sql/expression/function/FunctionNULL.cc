// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionNULL.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionNULL> nullFunctionBuilder("null");
static FunctionBuilder<FunctionNULL> isnullFunctionBuilder("isnull");

FunctionNULL::FunctionNULL(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionNULL::FunctionNULL(const FunctionNULL& other) : FunctionExpression(other.name_, other.args_) {}

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

}  // namespace eckit::sql::expression::function
