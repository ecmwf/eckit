// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionNOT_IN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNOT_IN_H
#define FunctionNOT_IN_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNOT_IN : public FunctionExpression {
public:

    FunctionNOT_IN(const std::string&, const expression::Expressions&);
    FunctionNOT_IN(const FunctionNOT_IN&);
    ~FunctionNOT_IN();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return -1; }

private:

    // No copy allowed
    FunctionNOT_IN& operator=(const FunctionNOT_IN&);

    int size_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNOT_IN& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
