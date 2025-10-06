/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Dec 2003

#ifndef eckit_sql_ColumnExpression_H
#define eckit_sql_ColumnExpression_H

#include "eckit/sql/expression/SQLExpression.h"

namespace eckit::sql {

class SQLOutput;
class SQLTable;
class SQLColumn;

namespace expression {

//----------------------------------------------------------------------------------------------------------------------

class ColumnExpression : public SQLExpression {
public:

    ColumnExpression(const std::string&, const SQLTable*, int begin = -1, int end = -1);
    ColumnExpression(const std::string&, const std::string& tableReference, int begin = -1, int end = -1);
    ColumnExpression(const ColumnExpression&) = default;
    ~ColumnExpression();

    const SQLTable* table() { return table_; }
    const double* current() { return value_->first; }
    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;

    SQLExpression* nominalShift(int n) {
        nominalShift_ = n;
        return this;
    }

protected:

    const type::SQLType* type_;  // non-owning
    std::pair<const double*, bool>* value_;
    std::string columnName_;
    const SQLTable* table_;
    std::string tableReference_;
    std::string fullName_;
    int beginIndex_;
    int endIndex_;
    int nominalShift_;

    // -- Overridden methods
    void print(std::ostream& s) const override;
    void preprepare(SQLSelect& sql) override;
    void prepare(SQLSelect& sql) override;
    void updateType(SQLSelect& sql) override;
    void cleanup(SQLSelect& sql) override;
    double eval(bool& missing) const override;
    void eval(double* out, bool& missing) const override;
    std::string evalAsString(bool& missing) const override;
    bool isConstant() const override { return false; }
    void output(SQLOutput& s) const override;

    // We want to be able to construct a fully qualified column name, given a shortened one
    // (e.g. varno --> varno@body). But this needs to be overridable for BitExpressions
    // (e.g. datum_state.active --> datum_state.active@body even though the table column
    //       might only be datum_state@body. i.e. we extract the bit subcolumns)
    virtual std::string tableColumnToFullname(const SQLColumn& column) const;

private:

    ColumnExpression& operator=(const ColumnExpression&);

    // -- Overridden methods
    const type::SQLType* type() const override { return type_; }
    void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&, Expressions&) override;
    void tables(std::set<const SQLTable*>&) override;

    friend class SQLSelectFactory;

    // friend std::ostream& operator<<(std::ostream& s,const ColumnExpression& p)
    //	{ p.print(s); return s; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace eckit::sql

#endif
