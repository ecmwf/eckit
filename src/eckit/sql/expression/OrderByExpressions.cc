/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/OrderByExpressions.h"
#include "eckit/utils/StringTools.h"

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

const static std::vector<bool> nullAscending;

OrderByExpressions::OrderByExpressions(const OrderByExpressions& o) :
    Expressions(o), ascending_(o.ascending_) {}

OrderByExpressions::OrderByExpressions(const std::vector<bool>& ascending) :
    ascending_(ascending) {}

OrderByExpressions::OrderByExpressions() :
    ascending_(nullAscending) {}

OrderByExpressions::~OrderByExpressions() {}


bool OrderByExpressions::operator<(const OrderByExpressions& o) const {
    size_t n = size();
    // ASSERT(n == o.size());
    ASSERT(ascending_.size() == n || ascending_.empty());

    for (size_t i = 0; i < n; ++i) {
        bool asc = ascending_.empty() ? true : ascending_[i];

        const SQLExpression& left  = asc ? *(*this)[i] : *o[i];
        const SQLExpression& right = asc ? *o[i] : *(*this)[i];

        if (left.type()->getKind() == type::SQLType::stringType) {

            if (right.type()->getKind() != type::SQLType::stringType)
                return false;

            bool missing1 = false;
            bool missing2 = false;

            std::string v1(left.evalAsString(missing1));
            std::string v2(right.evalAsString(missing2));

            if (missing1 != missing2)
                return missing1;

            v1 = StringTools::trim(v1, "\t\n\v\f\r ");
            v2 = StringTools::trim(v2, "\t\n\v\f\r ");

            if (v1 != v2)
                return (v1 < v2);
        }
        else {

            bool leftMissing  = false;
            bool rightMissing = false;

            double leftValue  = left.eval(leftMissing);
            double rightValue = right.eval(rightMissing);

            if (leftMissing != rightMissing)
                return leftMissing;

            if (leftValue != rightValue)
                return (leftValue < rightValue);
        }
    }

    // They are equal
    return false;
}

bool OrderByExpressions::operator==(const OrderByExpressions& o) const {

    size_t n = size();
    // ASSERT(n == o.size());
    ASSERT(ascending_.size() == n || ascending_.empty());

    for (size_t i = 0; i < n; ++i) {
        bool asc = ascending_[i];

        const SQLExpression& left  = asc ? *(*this)[i] : *o[i];
        const SQLExpression& right = asc ? *o[i] : *(*this)[i];

        if (left.type()->getKind() == type::SQLType::stringType) {

            if (right.type()->getKind() != type::SQLType::stringType)
                return false;

            bool missing1 = false;
            bool missing2 = false;

            std::string v1(left.evalAsString(missing1));
            std::string v2(right.evalAsString(missing2));

            if (missing1 != missing2)
                return false;

            v1 = StringTools::trim(v1, "\t\n\v\f\r ");
            v2 = StringTools::trim(v2, "\t\n\v\f\r ");

            if (v1 != v2)
                return false;
        }
        else {

            bool leftMissing  = false;
            bool rightMissing = false;

            double leftValue  = left.eval(leftMissing);
            double rightValue = right.eval(rightMissing);

            if (leftMissing != rightMissing)
                return false;
            if (leftValue != rightValue)
                return false;
        }
    }

    // They are equal
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace sql
}  // namespace eckit
