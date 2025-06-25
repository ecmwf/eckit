/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionJULIAN.h
// ECMWF July 2010

#ifndef FunctionJULIAN_H
#define FunctionJULIAN_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionJULIAN : public FunctionExpression {
public:

    FunctionJULIAN(const std::string&, const expression::Expressions&);
    FunctionJULIAN(const FunctionJULIAN&);
    ~FunctionJULIAN();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }

private:

    // No copy allowed
    FunctionJULIAN& operator=(const FunctionJULIAN&);

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionJULIAN& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
