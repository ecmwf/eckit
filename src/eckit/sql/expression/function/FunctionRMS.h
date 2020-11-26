/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionRMS.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionRMS_H
#define FunctionRMS_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionRMS : public FunctionExpression {
public:
    FunctionRMS(const std::string&, const expression::Expressions&);
    FunctionRMS(const FunctionRMS&);
    ~FunctionRMS();

    // -- Overridden methods
    virtual double eval(bool& missing) const override;

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }

private:
    // No copy allowed
    FunctionRMS& operator=(const FunctionRMS&);

    unsigned long long count_;
    double squares_;

    // -- Overridden methods
    virtual const eckit::sql::type::SQLType* type() const override;
    virtual void prepare(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual void partialResult() override;

    bool isAggregate() const override { return true; }

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionRMS& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
