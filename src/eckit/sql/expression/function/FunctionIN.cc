// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionIN.h"

#include "eckit/sql/expression/function/FunctionEQ.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionIN> inFunctionBuilder("in");

FunctionIN::FunctionIN(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), size_(args.size() - 1) {}

FunctionIN::FunctionIN(const FunctionIN& other) :
    FunctionExpression(other.name_, other.args_), size_(other.args_.size() - 1) {}

FunctionIN::~FunctionIN() {}

const type::SQLType* FunctionIN::type() const {
    return &type::SQLType::lookup("real");
}  // TODO: bool?

std::shared_ptr<SQLExpression> FunctionIN::clone() const {
    return std::make_shared<FunctionIN>(*this);
}

double FunctionIN::eval(bool& missing) const {
    const SQLExpression& x = *args_[size_];
    for (size_t i = 0; i < size_; ++i) {
        if (FunctionEQ::equal(x, *args_[i], missing)) {
            return true;
        }
    }
    return false;
}

}  // namespace eckit::sql::expression::function
