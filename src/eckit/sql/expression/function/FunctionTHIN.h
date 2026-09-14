// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// \file FunctionTHIN.h
/// ECMWF July 2010

#ifndef FunctionTHIN_H
#define FunctionTHIN_H

#include "eckit/sql/expression/function/FunctionIntegerExpression.h"

namespace eckit::sql::expression::function {

class FunctionTHIN : public FunctionIntegerExpression {
public:

    FunctionTHIN(const std::string&, const expression::Expressions&);
    FunctionTHIN(const FunctionTHIN&);
    ~FunctionTHIN();

    std::shared_ptr<SQLExpression> clone() const override;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;

    static int arity() { return 2; }

protected:

    // -- Overridden methods
    void print(std::ostream& s) const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    bool isConstant() const override;
    using FunctionIntegerExpression::eval;
    double eval(bool& missing) const override;
    std::shared_ptr<SQLExpression> simplify(bool&) override;
    bool isAggregate() const override { return false; }

private:

    // No copy allowed
    FunctionTHIN& operator=(const FunctionTHIN&);

    unsigned long long* count_;
    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionTHIN& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
