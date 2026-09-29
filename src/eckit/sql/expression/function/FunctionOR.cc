// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionOR.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionOR> orFunctionBuilder("or");

FunctionOR::FunctionOR(const std::string& name, const expression::Expressions& args) : FunctionExpression(name, args) {}

FunctionOR::FunctionOR(const FunctionOR& other) : FunctionExpression(other.name_, other.args_) {}

FunctionOR::~FunctionOR() {}

std::shared_ptr<SQLExpression> FunctionOR::clone() const {
    return std::make_shared<FunctionOR>(*this);
}

const type::SQLType* FunctionOR::type() const {
    return &type::SQLType::lookup("real");
}

double FunctionOR::eval(bool& missing) const {
    return args_[0]->eval(missing) || args_[1]->eval(missing);
}

std::shared_ptr<SQLExpression> FunctionOR::simplify(bool& changed) {
    std::shared_ptr<SQLExpression> x = FunctionExpression::simplify(changed);
    if (x) {
        return x;
    }

    for (int i = 0; i < 2; i++) {
        bool missing = false;
        if (args_[i]->isConstant()) {
            if (args_[i]->eval(missing)) {
                std::cout << "SYMPLIFY " << *this << " to 1" << std::endl;
                ;
                changed = true;
                return SQLExpression::number(1);
            }
        }
    }

    return nullptr;
}

}  // namespace eckit::sql::expression::function
