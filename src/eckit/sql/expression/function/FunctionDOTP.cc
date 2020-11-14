/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionDOTP.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

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
    if (resultNULL_)
        missing = true;
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

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
