/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
