/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionAVG.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionAVG_H
#define FunctionAVG_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionAVG : public FunctionExpression {
public:

    FunctionAVG(const std::string&, const expression::Expressions&);
    FunctionAVG(const FunctionAVG&);
    ~FunctionAVG();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }
    static const char* help() { return "Average (aggregate function)"; }

private:

    // No copy allowed
    FunctionAVG& operator=(const FunctionAVG&);

    // -- Members
    unsigned long long count_;
    double value_;

    // -- Overridden methods
    const type::SQLType* type() const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    void partialResult() override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    bool isAggregate() const override { return true; }

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionAVG& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
