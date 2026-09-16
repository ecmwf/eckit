// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cfloat>
#include <climits>

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionMAX.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionMAX> maxFunctionBuilder("max");

FunctionMAX::FunctionMAX(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), value_(-DBL_MAX) {}

FunctionMAX::FunctionMAX(const FunctionMAX& other) :
    FunctionExpression(other.name_, other.args_), value_(other.value_) {}

std::shared_ptr<SQLExpression> FunctionMAX::clone() const {
    return std::make_shared<FunctionMAX>(*this);
}

const eckit::sql::type::SQLType* FunctionMAX::type() const {
    return args_[0]->type();
}

FunctionMAX::~FunctionMAX() {}

double FunctionMAX::eval(bool& missing) const {
    if (value_ == -DBL_MAX) {
        missing = true;
    }

    return value_;
}

void FunctionMAX::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    value_ = -DBL_MAX;
}

void FunctionMAX::output(SQLOutput& s) const {
    bool missing = false;
    double d     = eval(missing);
    type()->output(s, d, missing);
}

void FunctionMAX::cleanup(SQLSelect& sql) {
    FunctionExpression::cleanup(sql);
    value_ = -DBL_MAX;
}

void FunctionMAX::partialResult() {
    bool missing = false;
    double value = args_[0]->eval(missing);
    if (!missing) {
        if (value > value_) {
            value_ = value;
        }
    }
}

}  // namespace eckit::sql::expression::function
