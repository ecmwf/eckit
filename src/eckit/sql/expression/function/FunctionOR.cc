/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/FunctionOR.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionOR> orFunctionBuilder("or");

FunctionOR::FunctionOR(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args) {}

FunctionOR::FunctionOR(const FunctionOR& other) :
    FunctionExpression(other.name_, other.args_) {}

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

    return 0;
}

}  // namespace eckit::sql::expression::function
