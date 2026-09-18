// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
