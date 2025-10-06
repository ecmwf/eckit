/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date Nov 11

#ifndef eckit_api_Expressions_H
#define eckit_api_Expressions_H

#include <memory>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/SQLExpression.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

// n.b. shared pointer not unique_ptr.
//      needs to be copyable to be used by YACC
typedef std::vector<std::shared_ptr<SQLExpression>> ExpressionsVector;

class Expressions : public SQLExpression, public ExpressionsVector {
public:

    Expressions() : ExpressionsVector() {}
    Expressions(size_t i) : ExpressionsVector(i, 0) {}

    Expressions(const Expressions&)            = default;
    Expressions& operator=(const Expressions&) = default;

    Expressions(Expressions&&)            = default;
    Expressions& operator=(Expressions&&) = default;

    void print(std::ostream& s) const override;

    friend std::ostream& operator<<(std::ostream& o, const Expressions& e) {
        e.print(o);
        return o;
    }

    //////////////////////////////////////////////////////////////////////////////////////

    void prepare(SQLSelect&) override {}
    void cleanup(SQLSelect&) override {}

    const type::SQLType* type() const override;

    // -- For WHERE
    using SQLExpression::eval;
    double eval(bool& missing) const override { NOTIMP; }

    bool isConstant() const override { NOTIMP; }
    bool isNumber() const override { return false; }
    virtual bool isVector() const { return true; }
    virtual Expressions& vector() { return *this; }

    std::shared_ptr<SQLExpression> simplify(bool&) override { return shared_from_this(); }

    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift_) const override;
    virtual Expressions reshift_expressions(int minColumnShift_) const;

    bool isAggregate() const override { return false; }
    // For select expression

    void output(SQLOutput&) const override { return NOTIMP; }
    void partialResult() override {}
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&,
                             expression::Expressions&) override {
        NOTIMP;
    }
    //////////////////////////////////////////////////////////////////////////////////////
};

typedef std::vector<Expressions> VectorOfExpressions;
// class VectorOfExpressions : public vector<Expressions> { public: ~VectorOfExpressions() { for (size_t i=0; i <
// size(); ++i) ; } };

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
