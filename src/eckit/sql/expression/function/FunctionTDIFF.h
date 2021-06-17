/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionTDIFF.h
// ECMWF July 2010

#ifndef FunctionTDIFF_H
#define FunctionTDIFF_H

#include "eckit/sql/expression/function/FunctionIntegerExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

class FunctionTDIFF : public FunctionIntegerExpression {
public:
    FunctionTDIFF(const std::string&, const expression::Expressions&);
    FunctionTDIFF(const FunctionTDIFF&);
    ~FunctionTDIFF();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 4; }

private:
    // No copy allowed
    FunctionTDIFF& operator=(const FunctionTDIFF&);

    // -- Overridden methods
    virtual const eckit::sql::type::SQLType* type() const override;
    virtual double eval(bool& missing) const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionTDIFF& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
