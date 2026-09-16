// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionNVL.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionNVL> nvlFunctionBuilder("nvl");

FunctionNVL::FunctionNVL(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionNVL::FunctionNVL(const FunctionNVL& other) : FunctionExpression(other.name_, other.args_) {}

FunctionNVL::~FunctionNVL() {}

std::shared_ptr<SQLExpression> FunctionNVL::clone() const {
    return std::make_shared<FunctionNVL>(*this);
}

const type::SQLType* FunctionNVL::type() const {
    return &type::SQLType::lookup("real");
}

// Don't set the missing flag
double FunctionNVL::eval(bool&) const {
    bool missing = false;
    double x     = args_[0]->eval(missing);
    return missing ? args_[1]->eval(missing) : x;
}

}  // namespace eckit::sql::expression::function
