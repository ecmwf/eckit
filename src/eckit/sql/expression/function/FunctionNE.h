// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionNE.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNE_H
#define FunctionNE_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNE : public FunctionExpression {
public:

    FunctionNE(const std::string&, const expression::Expressions&);
    FunctionNE(const FunctionNE&);
    ~FunctionNE();

    static bool equal(const SQLExpression& l, const SQLExpression& r, bool& missing);

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionNE& operator=(const FunctionNE&);

    double tmp_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNE& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
