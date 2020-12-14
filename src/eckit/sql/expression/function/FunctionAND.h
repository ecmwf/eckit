/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// @author Baudouin Raoult
// @author Simon Smart
// @date December 2003
// @date January 2019

#ifndef FunctionAND_H
#define FunctionAND_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

//----------------------------------------------------------------------------------------------------------------------

class FunctionAND : public FunctionExpression {
public:
    FunctionAND(const std::string&, const expression::Expressions&);
    FunctionAND(const FunctionAND&);
    ~FunctionAND();

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 2; }
    static const char* help() { return ""; }

private:
    // No copy allowed
    FunctionAND& operator=(const FunctionAND&);

    virtual const eckit::sql::type::SQLType* type() const override;
    virtual double eval(bool& missing) const override;
    virtual std::shared_ptr<SQLExpression> simplify(bool&) override;
    virtual bool andSplit(expression::Expressions&) override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionAND& p)
    //	{ p.print(s); return s; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
