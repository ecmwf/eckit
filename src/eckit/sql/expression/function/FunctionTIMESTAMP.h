// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionTIMESTAMP.h
// ECMWF July 2010

#ifndef FunctionTIMESTAMP_H
#define FunctionTIMESTAMP_H

#include "eckit/sql/expression/function/FunctionIntegerExpression.h"

namespace eckit::sql::expression::function {

class FunctionTIMESTAMP : public FunctionIntegerExpression {
public:

    FunctionTIMESTAMP(const std::string&, const expression::Expressions&);
    FunctionTIMESTAMP(const FunctionTIMESTAMP&);
    ~FunctionTIMESTAMP();

    std::shared_ptr<SQLExpression> clone() const override;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;

    static int arity() { return 2; }

private:

    FunctionTIMESTAMP& operator=(const FunctionTIMESTAMP&);

    // -- Overridden methods
    using FunctionIntegerExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionTIMESTAMP& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
