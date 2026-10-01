// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionVAR.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionVAR_H
#define FunctionVAR_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionVAR : public FunctionExpression {
public:

    FunctionVAR(const std::string&, const expression::Expressions&);
    FunctionVAR(const FunctionVAR&);
    ~FunctionVAR();

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    void partialResult() override;

    bool isAggregate() const override { return true; }

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

protected:

    // -- Overridden methods
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

private:

    // No copy allowed
    FunctionVAR& operator=(const FunctionVAR&);

    unsigned long long count_;
    double value_;
    double squares_;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionVAR& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
