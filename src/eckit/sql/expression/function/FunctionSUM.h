// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionSUM.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionSUM_H
#define FunctionSUM_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionSUM : public FunctionExpression {
public:

    FunctionSUM(const std::string&, const expression::Expressions&);
    FunctionSUM(const FunctionSUM&);
    ~FunctionSUM();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    FunctionSUM& operator=(const FunctionSUM&);

    unsigned long long count_;
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
    // friend std::ostream& operator<<(std::ostream& s,const FunctionSUM& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
