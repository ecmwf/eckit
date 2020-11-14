/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cfloat>
#include <climits>

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionLAST.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

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
    if (value_ == DBL_MAX)
        missing = true;

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

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
