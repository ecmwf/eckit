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
