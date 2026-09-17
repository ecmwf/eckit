// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionAND.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionAND> andFunctionBuilder("and");


FunctionAND::FunctionAND(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionAND::FunctionAND(const FunctionAND& other) : FunctionExpression(other.name_, other.args_) {}

std::shared_ptr<SQLExpression> FunctionAND::clone() const {
    return std::make_shared<FunctionAND>(*this);
}

FunctionAND::~FunctionAND() {}

const type::SQLType* FunctionAND::type() const {
    return &type::SQLType::lookup("double");
}  // TODO: bool?

double FunctionAND::eval(bool& missing) const {
    return args_[0]->eval(missing) && args_[1]->eval(missing);
}

bool FunctionAND::andSplit(expression::Expressions& e) {
    bool ok = false;

    if (!args_[0]->andSplit(e)) {
        e.push_back(args_[0]);
        ok = true;
    }

    if (!args_[1]->andSplit(e)) {
        e.push_back(args_[1]);
        ok = true;
    }

    return ok;
}

std::shared_ptr<SQLExpression> FunctionAND::simplify(bool& changed) {
    std::shared_ptr<SQLExpression> x = FunctionExpression::simplify(changed);
    if (x) {
        return x;
    }

    for (int i = 0; i < 2; i++) {
        bool missing = false;
        if (args_[i]->isConstant()) {
            if (args_[i]->eval(missing)) {
                std::cout << "SYMPLIFY " << *this << " to ";
                changed = true;

                std::shared_ptr<SQLExpression> x = args_[1 - i];
                args_.clear();

                std::cout << *x << std::endl;
                return x;
            }
            std::cout << "SYMPLIFY " << *this << "to 0 " << std::endl;
            changed = true;
            return SQLExpression::number(0);
        }
    }

    return nullptr;
}

}  // namespace eckit::sql::expression::function
