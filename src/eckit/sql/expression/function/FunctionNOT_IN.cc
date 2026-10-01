// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionNOT_IN.h"
#include "eckit/sql/expression/function/FunctionEQ.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionNOT_IN> not_inFunctionBuilder("not_in");

FunctionNOT_IN::FunctionNOT_IN(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), size_(args.size() - 1) {}

FunctionNOT_IN::FunctionNOT_IN(const FunctionNOT_IN& other) :
    FunctionExpression(other.name_, other.args_), size_(other.args_.size() - 1) {}

FunctionNOT_IN::~FunctionNOT_IN() {}

std::shared_ptr<SQLExpression> FunctionNOT_IN::clone() const {
    return std::make_shared<FunctionNOT_IN>(*this);
}

const type::SQLType* FunctionNOT_IN::type() const {
    return &type::SQLType::lookup("double");
}

double FunctionNOT_IN::eval(bool& missing) const {
    const SQLExpression& x = *args_[size_];
    for (int i = 0; i < size_; ++i) {
        args_[i]->eval(missing);
        if (FunctionEQ::equal(x, *args_[i], missing)) {
            return false;
        }
    }

    return true;
}

}  // namespace eckit::sql::expression::function
