/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionJULIAN_SECONDS.h
/// ECMWF February 2014

#ifndef FunctionJULIAN_SECONDS_H
#define FunctionJULIAN_SECONDS_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit::sql::expression::function {

class FunctionJULIAN_SECONDS : public FunctionExpression {
public:

    FunctionJULIAN_SECONDS(const std::string&, const expression::Expressions&);
    FunctionJULIAN_SECONDS(const FunctionJULIAN_SECONDS&);
    ~FunctionJULIAN_SECONDS();  // Change to virtual if base class

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }
    static const char* help() { return "Returns time in Julian calendar expressed in seconds"; }

private:

    // No copy allowed
    FunctionJULIAN_SECONDS& operator=(const FunctionJULIAN_SECONDS&);

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;
    using FunctionExpression::eval;
    double eval(bool& missing) const override;

    // -- Friends
    // friend ostream& operator<<(ostream& s,const FunctionJULIAN_SECONDS& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
