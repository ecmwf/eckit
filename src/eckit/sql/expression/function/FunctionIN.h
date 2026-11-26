// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionIN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionIN_H
#define FunctionIN_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionIN : public FunctionExpression {
public:

    FunctionIN(const std::string&, const expression::Expressions&);
    FunctionIN(const FunctionIN&);
    ~FunctionIN();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return -1; }

private:

    // No copy allowed
    FunctionIN& operator=(const FunctionIN&);

    size_t size_;

    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionIN& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
