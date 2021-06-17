/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionROWNUMBER.h
/// Piotr Kuchta - (C) ECMWF July 2009

#ifndef FunctionROWNUMBER_H
#define FunctionROWNUMBER_H

#include "eckit/sql/expression/function/FunctionIntegerExpression.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionROWNUMBER : public FunctionIntegerExpression {
public:
    FunctionROWNUMBER(const std::string&, const expression::Expressions&);
    FunctionROWNUMBER(const FunctionROWNUMBER&);
    ~FunctionROWNUMBER();

    // -- Overridden methods
    virtual const eckit::sql::type::SQLType* type() const override;

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 0; }

protected:
    // -- Overridden methods
    virtual void print(std::ostream& s) const override;
    virtual void prepare(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual bool isConstant() const override;
    virtual void partialResult() override;
    virtual double eval(bool& missing) const override;
    virtual std::shared_ptr<SQLExpression> simplify(bool&) override;
    bool isAggregate() const override { return false; }

private:
    // No copy allowed
    FunctionROWNUMBER& operator=(const FunctionROWNUMBER&);

    unsigned long long* count_;
    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionROWNUMBER& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
