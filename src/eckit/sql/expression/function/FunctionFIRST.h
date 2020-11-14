/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionFIRST.h
// Piotr Kuchta - ECMWF Nov 2016

#ifndef eckit_sql_function_FunctionFIRST_H
#define eckit_sql_function_FunctionFIRST_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionFIRST : public FunctionExpression {
public:
    FunctionFIRST(const std::string&, const expression::Expressions&);
    FunctionFIRST(const FunctionFIRST&);
    ~FunctionFIRST();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:
    // No copy allowed
    FunctionFIRST& operator=(const FunctionFIRST&);

    double value_;
    bool notFirst_;

    // -- Overridden methods
    virtual const eckit::sql::type::SQLType* type() const override;
    virtual void prepare(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual void partialResult() override;
    virtual double eval(bool& missing) const override;
    bool isAggregate() const override { return true; }

    virtual void output(SQLOutput&) const override;
    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionFIRST& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
