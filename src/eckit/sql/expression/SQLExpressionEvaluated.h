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

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

class SQLExpressionEvaluated : public SQLExpression {
public:
    SQLExpressionEvaluated(SQLExpression&);
    virtual ~SQLExpressionEvaluated() override;

    // Overriden

    virtual void prepare(SQLSelect&) override;
    virtual void updateType(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual double eval(bool& missing) const override;
    virtual void eval(double* out, bool& missing) const override;
    virtual std::string evalAsString(bool& missing) const override;
    virtual bool isConstant() const override;
    virtual bool isNumber() const override;
    virtual std::shared_ptr<SQLExpression> simplify(bool&) override;
    virtual std::shared_ptr<SQLExpression> clone() const override;
    virtual std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;
    virtual bool isAggregate() const override;

    virtual const type::SQLType* type() const override;

    virtual void output(SQLOutput& o) const override;

protected:
    virtual void print(std::ostream&) const override;

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

}  // namespace expression
}  // namespace sql
}  // namespace eckit


#endif
