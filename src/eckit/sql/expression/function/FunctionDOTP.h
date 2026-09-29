// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionDOTP.h
// ECMWF July 2010

#ifndef FunctionDOTP_H
#define FunctionDOTP_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionDOTP : public FunctionExpression {
public:

    FunctionDOTP(const std::string&, const expression::Expressions&);
    FunctionDOTP(const FunctionDOTP&);
    ~FunctionDOTP();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionDOTP& operator=(const FunctionDOTP&);

    double value_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    void partialResult() override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    bool isAggregate() const override { return true; }

    bool resultNULL_;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionDOTP& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
