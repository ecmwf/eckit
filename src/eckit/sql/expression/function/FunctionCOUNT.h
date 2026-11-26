// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionCOUNT.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionCOUNT_H
#define FunctionCOUNT_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionCOUNT : public FunctionExpression {
public:

    FunctionCOUNT(const std::string&, const expression::Expressions&);
    FunctionCOUNT(const FunctionCOUNT&);
    ~FunctionCOUNT();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }
    static const char* help() { return "Average (aggregate function)"; }

private:

    // No copy allowed
    FunctionCOUNT& operator=(const FunctionCOUNT&);

    unsigned long long count_;

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    void partialResult() override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    bool isAggregate() const override { return true; }

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionCOUNT& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
