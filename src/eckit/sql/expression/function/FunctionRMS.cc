/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/expression/function/FunctionRMS.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionRMS> rmsFunctionBuilder("rms");

FunctionRMS::FunctionRMS(const std::string& name, const expression::Expressions& args) :
    FunctionExpression(name, args), count_(0), squares_(0) {}

FunctionRMS::FunctionRMS(const FunctionRMS& other) :
    FunctionExpression(other.name_, other.args_), count_(other.count_), squares_(other.squares_) {}

FunctionRMS::~FunctionRMS() {}

std::shared_ptr<SQLExpression> FunctionRMS::clone() const {
    return std::make_shared<FunctionRMS>(*this);
}

const type::SQLType* FunctionRMS::type() const {
    return &type::SQLType::lookup("double");
}

double FunctionRMS::eval(bool& missing) const {
    if (!count_) {
        missing = true;
        return 0;
    }

    return sqrt(squares_ / count_);
}

void FunctionRMS::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);
    count_   = 0;
    squares_ = 0;
}

void FunctionRMS::cleanup(SQLSelect& sql) {
    // cout << "Cleanup  FunctionRMS " << count_ << " " << value_ << std::endl;
    FunctionExpression::cleanup(sql);
    squares_ = 0;
    count_   = 0;
}

void FunctionRMS::partialResult() {
    bool missing = false;
    double value = args_[0]->eval(missing);
    if (!missing) {
        squares_ += value * value;
        count_++;
    }
    //	else cout << "missing" << std::endl;
}


}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
