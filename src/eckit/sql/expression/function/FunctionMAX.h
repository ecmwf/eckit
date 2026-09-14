// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionMAX.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionMAX_H
#define FunctionMAX_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionMAX : public FunctionExpression {
public:

    FunctionMAX(const std::string&, const expression::Expressions&);
    FunctionMAX(const FunctionMAX&);
    ~FunctionMAX();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    FunctionMAX& operator=(const FunctionMAX&);

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
    // friend std::ostream& operator<<(std::ostream& s,const FunctionMAX& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
