// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/SQLExpressionEvaluated.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

SQLExpressionEvaluated::SQLExpressionEvaluated(SQLExpression& e) :
    type_(e.type()), missing_(false), missingValue_(e.missingValue()) {

    size_t byteSize = type_->size();
    ASSERT(byteSize % sizeof(double) == 0);
    value_.resize(byteSize / sizeof(double));

    e.eval(&value_[0], missing_);
    hasMissingValue_ = e.hasMissingValue();
}

SQLExpressionEvaluated::~SQLExpressionEvaluated() {}

void SQLExpressionEvaluated::print(std::ostream& o) const {
    if (missing_) {
        o << "NULL";
    }
    else {
        o << type_->asString(&value_[0]);
    }
    o << ", ";
}

double SQLExpressionEvaluated::eval(bool& missing) const {
    if (missing_) {
        missing = true;
    }
    return value_[0];
}

void SQLExpressionEvaluated::eval(double* out, bool& missing) const {
    if (missing_) {
        missing = true;
    }
    ::memcpy(out, &value_[0], value_.size() * sizeof(value_[0]));
}

std::string SQLExpressionEvaluated::evalAsString(bool& missing) const {
    if (missing_) {
        missing = true;
    }
    else {
        return type_->asString(&value_[0]);
    }
    return std::string();
}

void SQLExpressionEvaluated::output(SQLOutput& o) const {
    type_->output(o, &value_[0], missing_);
}

void SQLExpressionEvaluated::prepare(SQLSelect&) {
    NOTIMP;
}

void SQLExpressionEvaluated::updateType(SQLSelect&) {
    NOTIMP;
}

void SQLExpressionEvaluated::cleanup(SQLSelect&) {
    NOTIMP;
}
bool SQLExpressionEvaluated::isAggregate() const {
    NOTIMP;
}

bool SQLExpressionEvaluated::isConstant() const {
    return true;
}

bool SQLExpressionEvaluated::isNumber() const {
    return type_->getKind() != type::SQLType::stringType;
}

std::shared_ptr<SQLExpression> SQLExpressionEvaluated::simplify(bool&) {
    return nullptr;
}

std::shared_ptr<SQLExpression> SQLExpressionEvaluated::clone() const {
    return std::make_shared<SQLExpressionEvaluated>(*this);
}

std::shared_ptr<SQLExpression> SQLExpressionEvaluated::reshift(int minColumnShift) const {
    throw eckit::SeriousBug("Attempting to row-shift already-evaluated expression", Here());
}

const type::SQLType* SQLExpressionEvaluated::type() const {
    return type_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression
