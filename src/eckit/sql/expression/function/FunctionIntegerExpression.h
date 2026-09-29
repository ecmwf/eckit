// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionIntegerExpression.h
// ECMWF July 2010

#ifndef FUNCTION_INTEGER_EXPRESSION_H
#define FUNCTION_INTEGER_EXPRESSION_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql {

class SQLOutput;

namespace expression::function {

class FunctionIntegerExpression : public FunctionExpression {
public:

    static void registerIntegerFunctions();

    FunctionIntegerExpression(const std::string&, const expression::Expressions&);
    ~FunctionIntegerExpression();

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    void output(SQLOutput& s) const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    FunctionIntegerExpression(const FunctionIntegerExpression&);
    FunctionIntegerExpression& operator=(const FunctionIntegerExpression&);

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionIntegerExpression& p)
    //	{ p.print(s); return s; }
};

}  // namespace expression::function
}  // namespace eckit::sql

#endif
