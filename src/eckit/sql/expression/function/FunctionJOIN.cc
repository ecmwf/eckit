/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionJOIN.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionJOIN> joinFunctionBuilder("join");

FunctionJOIN::FunctionJOIN(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionJOIN::FunctionJOIN(const FunctionJOIN& other) :
    FunctionExpression(other.name_, other.args_) {}

std::shared_ptr<SQLExpression> FunctionJOIN::clone() const {
    return std::make_shared<FunctionJOIN>(*this);
}

FunctionJOIN::~FunctionJOIN() {}

const type::SQLType* FunctionJOIN::type() const {
    return &type::SQLType::lookup("real");
}

double FunctionJOIN::eval(bool& missing) const {
    return args_[0]->eval(missing) == args_[1]->eval(missing);
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
