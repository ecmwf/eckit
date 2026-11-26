// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionMIN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionMIN_H
#define FunctionMIN_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionMIN : public FunctionExpression {
public:

    FunctionMIN(const std::string&, const expression::Expressions&);
    FunctionMIN(const FunctionMIN&);
    ~FunctionMIN();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    FunctionMIN& operator=(const FunctionMIN&);

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
    // friend std::ostream& operator<<(std::ostream& s,const FunctionMIN& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
