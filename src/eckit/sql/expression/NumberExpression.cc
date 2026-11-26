// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/NumberExpression.h"

#include <ostream>

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

NumberExpression::NumberExpression(double value) : SQLExpression(), value_(value) {}

NumberExpression::NumberExpression(const NumberExpression& other) : SQLExpression(), value_(other.value_) {}

std::shared_ptr<SQLExpression> NumberExpression::clone() const {
    return std::make_shared<NumberExpression>(*this);
}

std::shared_ptr<SQLExpression> NumberExpression::reshift(int minColumnShift) const {
    return clone();
}

NumberExpression::~NumberExpression() {}

const type::SQLType* NumberExpression::type() const {
    return &type::SQLType::lookup("real");
}

double NumberExpression::eval(bool& missing) const {
    return value_;
}

void NumberExpression::prepare(SQLSelect& sql) {}

void NumberExpression::cleanup(SQLSelect& sql) {}

void NumberExpression::print(std::ostream& s) const {
    s << value_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression
