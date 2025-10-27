/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionNOT_NULL.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNOT_NULL_H
#define FunctionNOT_NULL_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionNOT_NULL : public FunctionExpression {
public:

    FunctionNOT_NULL(const std::string&, const expression::Expressions&);
    FunctionNOT_NULL(const FunctionNOT_NULL&);
    ~FunctionNOT_NULL();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:

    // No copy allowed
    const eckit::sql::type::SQLType* type() const override;
    FunctionNOT_NULL& operator=(const FunctionNOT_NULL&);

    // -- Overridden methods
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionNOT_NULL& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
