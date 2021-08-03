/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/NumberExpression.h"

#include <ostream>

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

NumberExpression::NumberExpression(double value) :
    value_(value) {}

NumberExpression::NumberExpression(const NumberExpression& other) :
    value_(other.value_) {}

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

}  // namespace expression
}  // namespace sql
}  // namespace eckit
