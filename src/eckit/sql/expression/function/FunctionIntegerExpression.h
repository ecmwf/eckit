/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionIntegerExpression.h
// ECMWF July 2010

#ifndef FUNCTION_INTEGER_EXPRESSION_H
#define FUNCTION_INTEGER_EXPRESSION_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {

class SQLOutput;

namespace expression {
namespace function {

class FunctionIntegerExpression : public FunctionExpression {
public:
    static void registerIntegerFunctions();

    FunctionIntegerExpression(const std::string&, const expression::Expressions&);
    ~FunctionIntegerExpression();

    // -- Overridden methods
    virtual const eckit::sql::type::SQLType* type() const override;
    virtual void output(SQLOutput& s) const override;

    static int arity() { return 1; }

private:
    // No copy allowed
    FunctionIntegerExpression(const FunctionIntegerExpression&);
    FunctionIntegerExpression& operator=(const FunctionIntegerExpression&);

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionIntegerExpression& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
