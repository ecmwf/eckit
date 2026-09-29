// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionOR.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionOR_H
#define FunctionOR_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionOR : public FunctionExpression {
public:

    FunctionOR(const std::string&, const expression::Expressions&);
    FunctionOR(const FunctionOR&);
    ~FunctionOR();

    // -- Overridden methods
    std::shared_ptr<SQLExpression> clone() const override;

    using FunctionExpression::eval;
    double eval(bool& missing) const override;
    const eckit::sql::type::SQLType* type() const override;
    std::shared_ptr<SQLExpression> simplify(bool&) override;

    static int arity() { return 2; }
    static const char* help() { return ""; }

private:

    // No copy allowed
    FunctionOR& operator=(const FunctionOR&);

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionOR& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
