/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLExpression_H
#define SQLExpression_H

#include <memory>
#include <set>

#include "eckit/sql/SQLTypedefs.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit::sql {
// Forward declarations

class SQLSelect;
class SQLTable;
class SQLOutput;

namespace expression {

class Expressions;
class SQLExpression;
class Dictionary;

class SQLExpression : public std::enable_shared_from_this<SQLExpression> {
public:

    SQLExpression();
    virtual ~SQLExpression();

    /// pre-prepare ideally wouldn't exist. To enable a two-pass prepare of SQL columns
    /// for SQLSelect, this is here.
    virtual void preprepare(SQLSelect&) {}
    virtual void prepare(SQLSelect&) = 0;
    virtual void updateType(SQLSelect&) {}
    virtual void cleanup(SQLSelect&) = 0;

    // @note Original SQL implementation used double for everything, but strings may
    // have more than 8 characters, so we need a way of getting bigger data out.
    // --> where relevant the eval(double*, bool&) method is used. Otherwise it just
    //     forwards to the normal eval.

    virtual double eval(bool& missing) const = 0;
    virtual void eval(double* out, bool& missing) const;
    virtual std::string evalAsString(bool& missing) const;

    virtual bool andSplit(expression::Expressions&) { return false; }
    virtual void tables(std::set<const SQLTable*>&) {}

    virtual bool isConstant() const = 0;
    virtual bool isNumber() const { return false; }

    virtual std::shared_ptr<SQLExpression> simplify(bool&);

    virtual void title(const std::string&);
    virtual std::string title() const;

    virtual const type::SQLType* type() const = 0;  // non-owning, do not delete result
                                                    // ----

    virtual std::shared_ptr<SQLExpression> clone() const                     = 0;
    virtual std::shared_ptr<SQLExpression> reshift(int minColumnShift) const = 0;

    virtual bool isAggregate() const { return false; }
    // For select expression

    virtual void output(SQLOutput&) const;
    virtual void partialResult() {}
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&, expression::Expressions&);

    virtual bool isBitfield() const { return isBitfield_; }
    BitfieldDef bitfieldDef() const { return bitfieldDef_; }

    virtual bool hasMissingValue() const { return hasMissingValue_; }
    double missingValue() const { return missingValue_; }

    static std::shared_ptr<SQLExpression> number(double);

    virtual void print(std::ostream&) const = 0;

protected:

    SQLExpression(const SQLExpression&)            = default;
    SQLExpression& operator=(const SQLExpression&) = default;

    bool isBitfield_;
    BitfieldDef bitfieldDef_;
    bool hasMissingValue_;
    double missingValue_;
    // bool isVector_;
    // Vector* vector_;

private:

    std::string title_;

    friend std::ostream& operator<<(std::ostream& s, const SQLExpression& p) {
        p.print(s);
        return s;
    }
};

}  // namespace expression
}  // namespace eckit::sql

// #include "eckit/sql/expression/SQLExpressions.h"

using namespace eckit::sql::expression;

#endif
