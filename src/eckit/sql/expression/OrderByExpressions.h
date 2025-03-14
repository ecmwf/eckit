/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Nov 2011
/// @date Aug 2018

#ifndef eckit_sql_expression_OrderByExpressions_H
#define eckit_sql_expression_OrderByExpressions_H

#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit::sql::expression {

/// @note This is fundamentally used only for the purpose of the SQLOrderOutput class

//----------------------------------------------------------------------------------------------------------------------

class OrderByExpressions : public Expressions {
public:

    OrderByExpressions(const OrderByExpressions& o);

    OrderByExpressions(const std::vector<bool>& ascending);
    OrderByExpressions();

    ~OrderByExpressions();

    bool operator<(const OrderByExpressions&) const;

    bool operator==(const OrderByExpressions&) const;

private:

    const std::vector<bool>& ascending_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
