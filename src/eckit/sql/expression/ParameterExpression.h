// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Dec 2003

#ifndef eckit_sql_ParameterExpression_H
#define eckit_sql_ParameterExpression_H

#include "eckit/sql/expression/SQLExpression.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

class ParameterExpression : public SQLExpression {
public:

    ParameterExpression(int);
    ParameterExpression(const ParameterExpression&);
    ~ParameterExpression();

    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override { return clone(); }

private:

    // No copy allowed
    ParameterExpression& operator=(const ParameterExpression&);

    // -- Members
    // None
    double value_;
    int which_;

    void print(std::ostream& s) const override;
    void prepare(SQLSelect& sql) override;
    void cleanup(SQLSelect& sql) override;

    using SQLExpression::eval;
    double eval(bool& missing) const override;
    const type::SQLType* type() const override;
    bool isConstant() const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
