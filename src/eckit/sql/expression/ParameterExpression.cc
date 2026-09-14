// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/ParameterExpression.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLSession.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

ParameterExpression::ParameterExpression(int which) : SQLExpression(), value_(0), which_(which) {
    // don't use any Log::* here
    //	std::cout << "new ParameterExpression " << name << std::endl;
}

ParameterExpression::ParameterExpression(const ParameterExpression& other) :
    SQLExpression(), value_(other.value_), which_(other.which_) {}


std::shared_ptr<SQLExpression> ParameterExpression::ParameterExpression::clone() const {
    return std::make_shared<ParameterExpression>(*this);
}

ParameterExpression::~ParameterExpression() {}

// TODO: are only real parameters allowed?
const type::SQLType* ParameterExpression::type() const {
    return &type::SQLType::lookup("real");
}

double ParameterExpression::eval(bool& missing) const {
    return value_;
}

void ParameterExpression::prepare(SQLSelect& sql) {
    NOTIMP;
    // TODO:
    // value_ = SQLSession::current().getParameter(which_);
    //	std::cout << "ParameterExpression " << name_ << " " << value_ << std::endl;
}

void ParameterExpression::cleanup(SQLSelect& sql) {
    value_ = 0;
}

void ParameterExpression::print(std::ostream& s) const {
    s << '?' << which_ << '=' << value_;
}

bool ParameterExpression::isConstant() const {
    return false;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression
