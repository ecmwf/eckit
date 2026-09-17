// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionNOT_NULL.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionNOT_NULL> not_nullFunctionBuilder("not_null");

FunctionNOT_NULL::FunctionNOT_NULL(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionNOT_NULL::FunctionNOT_NULL(const FunctionNOT_NULL& other) : FunctionExpression(other.name_, other.args_) {}

FunctionNOT_NULL::~FunctionNOT_NULL() {}

std::shared_ptr<SQLExpression> FunctionNOT_NULL::clone() const {
    return std::make_shared<FunctionNOT_NULL>(*this);
}

const type::SQLType* FunctionNOT_NULL::type() const {
    return &type::SQLType::lookup("real");
}

// Don't set the missing flags
double FunctionNOT_NULL::eval(bool&) const {
    bool missing = false;
#if 0
	std::cout << "FunctionNOT_NULL " << (*args_[0])  << " " << args_[0]->eval(missing);
	std::cout << " missing = " << missing << std::endl;
#else
    args_[0]->eval(missing);
#endif

    return !missing;
}

}  // namespace eckit::sql::expression::function
