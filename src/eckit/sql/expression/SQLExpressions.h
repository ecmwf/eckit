// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
using ExpressionsVector = std::vector<std::shared_ptr<SQLExpression>>;

class Expressions : public SQLExpression, public ExpressionsVector {
public:

    Expressions() : ExpressionsVector() {}
    Expressions(size_t i) : ExpressionsVector(i, nullptr) {}

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

using VectorOfExpressions = std::vector<Expressions>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
