/*
 * (C) Copyright 1996-2012 ECMWF.
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
#include "eckit/sql/expression/function/FunctionMIN.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionMIN> minFunctionBuilder("min");

FunctionMIN::FunctionMIN(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), value_(DBL_MAX) {}

FunctionMIN::FunctionMIN(const FunctionMIN& other) :
    FunctionExpression(other.name_, other.args_), value_(other.value_) {}

std::shared_ptr<SQLExpression> FunctionMIN::clone() const {
    return std::make_shared<FunctionMIN>(*this);
}

const eckit::sql::type::SQLType* FunctionMIN::type() const {
    return args_[0]->type();
}

FunctionMIN::~FunctionMIN() {}

double FunctionMIN::eval(bool& missing) const {
    if (value_ == DBL_MAX)
        missing = true;

    return value_;
}

void FunctionMIN::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    value_ = DBL_MAX;
}

void FunctionMIN::cleanup(SQLSelect& sql) {
    FunctionExpression::cleanup(sql);
    value_ = DBL_MAX;
}

void FunctionMIN::output(SQLOutput& s) const {
    bool missing = false;
    double d     = eval(missing);
    type()->output(s, d, missing);
}

void FunctionMIN::partialResult() {
    bool missing = false;
    double value = args_[0]->eval(missing);
    if (!missing)
        if (value < value_)
            value_ = value;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
