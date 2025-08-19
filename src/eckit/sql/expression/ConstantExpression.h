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
/// ECMWF Oct 11

#ifndef eckit_sql_ConstantExpression_H
#define eckit_sql_ConstantExpression_H

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/SQLExpression.h"

namespace eckit::sql {

class SQLSelect;
class SQLTable;
class SQLOutput;

namespace expression {

//----------------------------------------------------------------------------------------------------------------------

class ConstantExpression : public SQLExpression {

    struct PrivateKey {};  // This is just to allow make_shared to access private constructor.

public:

    ConstantExpression(double, bool, const type::SQLType*);
    ConstantExpression(const ConstantExpression&, const PrivateKey&);
    ConstantExpression& operator=(const ConstantExpression&) = delete;

    ~ConstantExpression() override;

    void prepare(SQLSelect&) override { NOTIMP; }
    void cleanup(SQLSelect&) override { NOTIMP; }

    // -- For WHERE
    using SQLExpression::eval;
    double eval(bool& missing) const override {
        missing = missing_;
        return value_;
    }

    bool isConstant() const override { return true; }
    bool isNumber() const override { NOTIMP; }

    // virtual SQLExpression* simplify(bool&);
    // virtual void title(const std::string&);
    // std::string title() const override;

    const type::SQLType* type() const override;
    // ----

    std::shared_ptr<SQLExpression> clone() const override {
        return std::make_shared<ConstantExpression>(*this, PrivateKey());
    }
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override { return clone(); }

    bool isAggregate() const override { return false; }
    // For select expression

    void output(SQLOutput& s) const override;
    void partialResult() override { NOTIMP; }
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&,
                             expression::Expressions&) override {
        NOTIMP;
    }

    bool isBitfield() const override { return isBitfield_; }
    BitfieldDef bitfieldDef() const { return bitfieldDef_; }
    bool hasMissingValue() const override { return hasMissingValue_; }
    double missingValue() const { return missingValue_; }

protected:

    void print(std::ostream&) const override { NOTIMP; }

    bool isBitfield_;
    BitfieldDef bitfieldDef_;
    bool hasMissingValue_;
    double missingValue_;

    double value_;
    bool missing_;
    const type::SQLType& type_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace eckit::sql

#endif
