// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/expression/ConstantExpression.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

ConstantExpression::ConstantExpression(double v, bool missing, const type::SQLType* type) :
    value_(v), missing_(missing), type_(*type) {}

ConstantExpression::ConstantExpression(const ConstantExpression& rhs, const ConstantExpression::PrivateKey&) :
    isBitfield_(rhs.isBitfield_),
    bitfieldDef_(rhs.bitfieldDef_),
    hasMissingValue_(rhs.hasMissingValue_),
    missingValue_(rhs.missingValue_),
    value_(rhs.value_),
    missing_(rhs.missing_),
    type_(rhs.type_) {}

ConstantExpression::~ConstantExpression() {}

void ConstantExpression::output(SQLOutput& o) const {
    type_.output(o, value_, missing_);
}

const type::SQLType* ConstantExpression::type() const {
    return &type_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression
