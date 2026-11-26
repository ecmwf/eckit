// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionLAST.h
// Piotr Kuchta - ECMWF Nov 2016

#ifndef eckit_sql_function_FunctionLAST_H
#define eckit_sql_function_FunctionLAST_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionLAST : public FunctionExpression {
public:

    FunctionLAST(const std::string&, const expression::Expressions&);
    FunctionLAST(const FunctionLAST&);
    ~FunctionLAST();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }
    static const char* help() { return ""; }

private:

    // No copy allowed
    FunctionLAST& operator=(const FunctionLAST&);

    double value_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    void partialResult() override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;
    bool isAggregate() const override { return true; }

    void output(SQLOutput&) const override;
    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionLAST& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
