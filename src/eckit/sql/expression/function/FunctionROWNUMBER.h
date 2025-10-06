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

namespace eckit::sql::expression::function {

class FunctionROWNUMBER : public FunctionIntegerExpression {
public:

    FunctionROWNUMBER(const std::string&, const expression::Expressions&);
    FunctionROWNUMBER(const FunctionROWNUMBER&);
    ~FunctionROWNUMBER();

    // -- Overridden methods
    const eckit::sql::type::SQLType* type() const override;

    std::shared_ptr<SQLExpression> clone() const override;

    static int arity() { return 0; }

protected:

    // -- Overridden methods
    void print(std::ostream& s) const override;
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    bool isConstant() const override;
    void partialResult() override;
    using FunctionIntegerExpression::eval;
    double eval(bool& missing) const override;
    std::shared_ptr<SQLExpression> simplify(bool&) override;
    bool isAggregate() const override { return false; }

private:

    // No copy allowed
    FunctionROWNUMBER& operator=(const FunctionROWNUMBER&);

    unsigned long long* count_;
    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionROWNUMBER& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
