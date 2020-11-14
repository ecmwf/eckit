/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StringExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef eckit_sql_StringExpression_H
#define eckit_sql_StringExpression_H

#include "eckit/sql/expression/SQLExpression.h"

namespace eckit {
namespace sql {
namespace expression {

//----------------------------------------------------------------------------------------------------------------------

class StringExpression : public SQLExpression {
public:
    StringExpression(const std::string&);
    StringExpression(const StringExpression&);
    ~StringExpression();

    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;

private:
    // No copy allowed
    StringExpression& operator=(const StringExpression&);

    std::string name_;
    std::vector<double> value_;
    const type::SQLType* type_;  // non-owning

    // -- Overridden methods
    virtual void print(std::ostream& s) const override;
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&,
                             expression::Expressions&) override;
    virtual void prepare(SQLSelect& sql) override;
    virtual void cleanup(SQLSelect& sql) override;

    virtual const type::SQLType* type() const override;
    virtual double eval(bool& missing) const override;
    virtual void eval(double* out, bool& missing) const override;
    virtual std::string evalAsString(bool& missing) const override;
    virtual bool isConstant() const override { return true; }
    virtual bool isNumber() const override { return true; }
    virtual void output(SQLOutput& o) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
