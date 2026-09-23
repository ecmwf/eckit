// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionLAST.h"

#include <cfloat>
#include <climits>

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionLAST> lastFunctionBuilder("last");

FunctionLAST::FunctionLAST(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), value_(DBL_MAX) {}

FunctionLAST::FunctionLAST(const FunctionLAST& other) :
    FunctionExpression(other.name_, other.args_), value_(other.value_) {}

std::shared_ptr<SQLExpression> FunctionLAST::clone() const {
    return std::make_shared<FunctionLAST>(*this);
}

const eckit::sql::type::SQLType* FunctionLAST::type() const {
    return args_[0]->type();
}

FunctionLAST::~FunctionLAST() {}

double FunctionLAST::eval(bool& missing) const {
    if (value_ == DBL_MAX) {
        missing = true;
    }

    return value_;
}

void FunctionLAST::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    value_ = DBL_MAX;
}

void FunctionLAST::cleanup(SQLSelect& sql) {
    FunctionExpression::cleanup(sql);
    value_ = DBL_MAX;
}

void FunctionLAST::output(SQLOutput& s) const {
    bool missing(false);
    double d(eval(missing));
    type()->output(s, d, missing);
}

void FunctionLAST::partialResult() {
    bool missing(false);
    value_ = (args_[0]->eval(missing));
}

}  // namespace eckit::sql::expression::function
