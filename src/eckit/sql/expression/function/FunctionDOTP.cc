// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionDOTP.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionDOTP> dotpFunctionBuilder("dotp");

const type::SQLType* FunctionDOTP::type() const {
    return &type::SQLType::lookup("double");
}

FunctionDOTP::FunctionDOTP(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), value_(0), resultNULL_(true) {}

FunctionDOTP::FunctionDOTP(const FunctionDOTP& other) :
    FunctionExpression(other.name_, other.args_), value_(other.value_), resultNULL_(other.resultNULL_) {}

std::shared_ptr<SQLExpression> FunctionDOTP::clone() const {
    return std::make_shared<FunctionDOTP>(*this);
}

FunctionDOTP::~FunctionDOTP() {}

double FunctionDOTP::eval(bool& missing) const {
    if (resultNULL_) {
        missing = true;
    }
    return value_;
}

void FunctionDOTP::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    value_ = 0;
}

void FunctionDOTP::cleanup(SQLSelect& sql) {
    FunctionExpression::cleanup(sql);
    value_ = 0;
}

void FunctionDOTP::partialResult() {
    bool missing = false;
    double x     = args_[0]->eval(missing);
    double y     = args_[1]->eval(missing);
    if (!missing) {
        value_ += x * y;
        resultNULL_ = false;
    }
}

}  // namespace eckit::sql::expression::function
