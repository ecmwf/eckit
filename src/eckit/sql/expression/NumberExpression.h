// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// @author Baudouin Raoult
// @author Simon Smart
// @date Dec 03

#ifndef eckit_sql_NumberExpression_H
#define eckit_sql_NumberExpression_H

#include "eckit/sql/expression/SQLExpression.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

class NumberExpression : public SQLExpression {
public:

    NumberExpression(double value);
    NumberExpression(const NumberExpression&);
    ~NumberExpression();

    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;

    void value(double v) { value_ = v; }

private:

    // No copy allowed
    NumberExpression& operator=(const NumberExpression&);

    double value_;

    // -- Overridden methods
    void print(std::ostream& s) const override;
    void prepare(SQLSelect& sql) override;
    void cleanup(SQLSelect& sql) override;

    const type::SQLType* type() const override;
    using SQLExpression::eval;
    double eval(bool& missing) const override;
    bool isConstant() const override { return true; }
    bool isNumber() const override { return true; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
