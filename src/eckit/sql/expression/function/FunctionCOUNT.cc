/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionCOUNT.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionCOUNT> countFunctionBuilder("count");

const type::SQLType* FunctionCOUNT::type() const {
    const type::SQLType& x = type::SQLType::lookup("double");
    return &x;
}

FunctionCOUNT::FunctionCOUNT(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), count_(0) {}

FunctionCOUNT::FunctionCOUNT(const FunctionCOUNT& other) :
    FunctionExpression(other.name_, other.args_), count_(other.count_) {}

std::shared_ptr<SQLExpression> FunctionCOUNT::clone() const {
    return std::make_shared<FunctionCOUNT>(*this);
}

FunctionCOUNT::~FunctionCOUNT() {}

double FunctionCOUNT::eval(bool& missing) const {
    // cout << "FunctionCOUNT " << count_ << std::endl;
    return count_;
}

void FunctionCOUNT::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    count_ = 0;
}

void FunctionCOUNT::cleanup(SQLSelect& sql) {
    FunctionExpression::cleanup(sql);
    count_ = 0;
}

void FunctionCOUNT::partialResult() {
    bool missing = false;
    args_[0]->eval(missing);
    if (!missing)
        count_++;
    // cout << "FunctionCOUNT::partialResult " << count_ << std::endl;
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
