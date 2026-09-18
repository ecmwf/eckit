// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionNVL.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNVL_H
#define FunctionNVL_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNVL : public FunctionExpression {
public:

    FunctionNVL(const std::string&, const expression::Expressions&);
    FunctionNVL(const FunctionNVL&);
    ~FunctionNVL();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionNVL& operator=(const FunctionNVL&);

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNVL& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
