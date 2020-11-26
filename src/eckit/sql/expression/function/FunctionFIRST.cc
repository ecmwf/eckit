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

#include "eckit/sql/expression/function/FunctionFIRST.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionFIRST> firstFunctionBuilder("first");

FunctionFIRST::FunctionFIRST(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), value_(DBL_MAX), notFirst_(false) {}

FunctionFIRST::FunctionFIRST(const FunctionFIRST& other) :
    FunctionExpression(other.name_, other.args_), value_(other.value_), notFirst_(other.notFirst_) {}

std::shared_ptr<SQLExpression> FunctionFIRST::clone() const {
    return std::make_shared<FunctionFIRST>(*this);
}

const eckit::sql::type::SQLType* FunctionFIRST::type() const {
    return args_[0]->type();
}

FunctionFIRST::~FunctionFIRST() {}

double FunctionFIRST::eval(bool& missing) const {
    if (value_ == DBL_MAX)
        missing = true;

    return value_;
}

void FunctionFIRST::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    value_ = DBL_MAX;
}

void FunctionFIRST::cleanup(SQLSelect& sql) {
    FunctionExpression::cleanup(sql);
    value_ = DBL_MAX;
}

void FunctionFIRST::output(SQLOutput& s) const {
    bool missing(false);
    double d(eval(missing));
    type()->output(s, d, missing);
}

void FunctionFIRST::partialResult() {
    if (notFirst_) {
        return;
    }

    bool missing(false);
    value_    = (args_[0]->eval(missing));
    notFirst_ = true;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
