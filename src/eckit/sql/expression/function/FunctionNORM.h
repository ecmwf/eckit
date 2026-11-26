// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FunctionNORM.h
// ECMWF July 2010

#ifndef FunctionNORM_H
#define FunctionNORM_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNORM : public FunctionExpression {
public:

    FunctionNORM(const std::string&, const expression::Expressions&);
    FunctionNORM(const FunctionNORM&);
    ~FunctionNORM();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionNORM& operator=(const FunctionNORM&);

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
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNORM& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
