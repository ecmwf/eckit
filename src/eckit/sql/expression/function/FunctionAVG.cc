// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/function/FunctionAVG.h"

#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit::sql::expression::function {

/* Static self-registration */

static FunctionBuilder<FunctionAVG> avgFunctionBuilder("avg");
static FunctionBuilder<FunctionAVG> meanFunctionBuilder("mean");

FunctionAVG::FunctionAVG(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), count_(0), value_(0) {}

FunctionAVG::FunctionAVG(const FunctionAVG& other) :
    FunctionExpression(other.name_, other.args_), count_(other.count_), value_(other.value_) {}


std::shared_ptr<SQLExpression> FunctionAVG::clone() const {
    return std::make_shared<FunctionAVG>(*this);
}

const type::SQLType* FunctionAVG::type() const {
    return &type::SQLType::lookup("double");
}

FunctionAVG::~FunctionAVG() {}

double FunctionAVG::eval(bool& missing) const {
    if (!count_) {
        missing = true;
        return 0;
    }

    return value_ / count_;
}

void FunctionAVG::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    value_ = 0;
    count_ = 0;
}

void FunctionAVG::cleanup(SQLSelect& sql) {
    // cout << "Cleanup  FunctionAVG " << count_ << " " << value_ << std::endl;
    FunctionExpression::cleanup(sql);
    value_ = 0;
    count_ = 0;
}

void FunctionAVG::partialResult() {
    bool missing = false;
    double value = args_[0]->eval(missing);
    if (!missing) {
        value_ += value;
        count_++;
    }
    //	else cout << "missing" << std::endl;
}

}  // namespace eckit::sql::expression::function
