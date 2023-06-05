/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionAND.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionAND> andFunctionBuilder("and");


FunctionAND::FunctionAND(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionAND::FunctionAND(const FunctionAND& other) :
    FunctionExpression(other.name_, other.args_) {}

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

    return 0;
}

}  // namespace eckit::sql::expression::function
