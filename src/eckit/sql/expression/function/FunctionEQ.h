// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionEQ.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionEQ_H
#define FunctionEQ_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionEQ : public FunctionExpression {
public:

    FunctionEQ(const std::string&, const expression::Expressions&);
    FunctionEQ(const FunctionEQ&);
    ~FunctionEQ();

    static bool equal(const SQLExpression& l, const SQLExpression& r, bool& missing);
    static void trimStringInDouble(char*& p, size_t& len);

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionEQ& operator=(const FunctionEQ&);

    double tmp_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;
    std::shared_ptr<SQLExpression> simplify(bool&) override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionEQ& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
