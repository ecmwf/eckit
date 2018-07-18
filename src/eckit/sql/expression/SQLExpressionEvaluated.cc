/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLExpressionEvaluated.h"

namespace eckit {
namespace sql {
namespace expression {

SQLExpressionEvaluated::SQLExpressionEvaluated(SQLExpression& e)
    : type_(e.type()),
      missing_(false),
      value_(e.eval(missing_)),
      missingValue_(e.missingValue())
{}

SQLExpressionEvaluated::~SQLExpressionEvaluated() {}

void SQLExpressionEvaluated::print(std::ostream& o) const
{
    if (missing_)
        o << "NULL";
    else
        o << value_;
    o << ", ";
}

void SQLExpressionEvaluated::prepare(SQLSelect&) { NOTIMP; }
void SQLExpressionEvaluated::cleanup(SQLSelect&) { NOTIMP; }
double SQLExpressionEvaluated::eval(bool& missing) const { if (missing_) missing = true; return value_; }
bool SQLExpressionEvaluated::isConstant() const { NOTIMP; /*?*/ return true; }
bool SQLExpressionEvaluated::isNumber() const { NOTIMP; /**/ return false; }
SQLExpression* SQLExpressionEvaluated::simplify(bool&) { NOTIMP; return 0; }
SQLExpression* SQLExpressionEvaluated::clone() const { NOTIMP; return 0; }
bool SQLExpressionEvaluated::isAggregate() const { NOTIMP; return false; }

const odb::sql::type::SQLType* SQLExpressionEvaluated::type() const { return type_; }

void SQLExpressionEvaluated::output(SQLOutput& o) const { type_->output(o, value_, missing_); }

} // namespace expression
} // namespace sql
} // namespace eckit

