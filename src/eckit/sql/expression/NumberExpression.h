/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
