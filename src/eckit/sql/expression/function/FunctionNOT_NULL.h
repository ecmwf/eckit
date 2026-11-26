// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionNOT_NULL.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNOT_NULL_H
#define FunctionNOT_NULL_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNOT_NULL : public FunctionExpression {
public:

    FunctionNOT_NULL(const std::string&, const expression::Expressions&);
    FunctionNOT_NULL(const FunctionNOT_NULL&);
    ~FunctionNOT_NULL();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    const eckit::sql::type::SQLType* type() const override;
    FunctionNOT_NULL& operator=(const FunctionNOT_NULL&);

    // -- Overridden methods
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNOT_NULL& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
