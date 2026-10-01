// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionNULL.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNULL_H
#define FunctionNULL_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNULL : public FunctionExpression {
public:

    FunctionNULL(const std::string&, const expression::Expressions&);
    FunctionNULL(const FunctionNULL&);
    ~FunctionNULL();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    const eckit::sql::type::SQLType* type() const override;
    FunctionNULL& operator=(const FunctionNULL&);

    // -- Overridden methods
    using FunctionExpression::eval;
    double eval(bool& missing) const override;
    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNULL& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
