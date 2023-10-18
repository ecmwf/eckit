/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionCOUNT.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionCOUNT_H
#define FunctionCOUNT_H

#include "eckit/sql/expression/function/FunctionIntegerExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionCOUNT : public FunctionIntegerExpression {
public:
    FunctionCOUNT(const std::string&, const expression::Expressions&);
    FunctionCOUNT(const FunctionCOUNT&);
    ~FunctionCOUNT() override = default;

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 1; }
    static const char* help() { return "Average (aggregate function)"; }

private:
    // No copy allowed
    FunctionCOUNT& operator=(const FunctionCOUNT&);

    int64_t count_;

    // -- Overridden methods
    virtual const eckit::sql::type::SQLType* type() const override;
    virtual void prepare(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual void partialResult() override;
    int64_t evalInt(bool& missing) const override;

    bool isAggregate() const override { return true; }

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionCOUNT& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
