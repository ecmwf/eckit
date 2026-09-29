// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Nov 2011
/// @date Aug 2018

#ifndef eckit_sql_expressiono_SQLExpressionEvaluated_H
#define eckit_sql_expressiono_SQLExpressionEvaluated_H

#include "eckit/sql/expression/SQLExpression.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

class SQLExpressionEvaluated : public SQLExpression {
public:

    SQLExpressionEvaluated(SQLExpression&);
    ~SQLExpressionEvaluated() override;

    // Overriden

    void prepare(SQLSelect&) override;
    void updateType(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    double eval(bool& missing) const override;
    void eval(double* out, bool& missing) const override;
    std::string evalAsString(bool& missing) const override;
    bool isConstant() const override;
    bool isNumber() const override;
    std::shared_ptr<SQLExpression> simplify(bool&) override;
    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;
    bool isAggregate() const override;

    const type::SQLType* type() const override;

    void output(SQLOutput& o) const override;

protected:

    void print(std::ostream&) const override;

    friend std::ostream& operator<<(std::ostream& s, const SQLExpressionEvaluated& p) {
        p.print(s);
        return s;
    }

    const type::SQLType* type_;
    bool missing_;
    std::vector<double> value_;
    double missingValue_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
