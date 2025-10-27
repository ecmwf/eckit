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

#ifndef eckit_sql_BitColumnExpression_H
#define eckit_sql_BitColumnExpression_H

#include "eckit/sql/expression/ColumnExpression.h"

namespace eckit::sql::expression {

//----------------------------------------------------------------------------------------------------------------------

class BitColumnExpression : public ColumnExpression {
public:

    BitColumnExpression(const std::string&, const std::string&, SQLTable*);
    BitColumnExpression(const std::string&, const std::string&, const std::string&);
    BitColumnExpression(const BitColumnExpression&);
    ~BitColumnExpression();

    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;

private:

    // No copy allowed
    BitColumnExpression& operator=(const BitColumnExpression&);

protected:

    unsigned long mask_;
    unsigned long bitShift_;

    std::string field_;
    std::string name_;

    // -- Overridden methods
    void prepare(SQLSelect& sql) override;
    void updateType(SQLSelect& sql) override;

    // Use SQLExpression's eval rather than ColumnExpression's
    void eval(double* out, bool& missing) const override { SQLExpression::eval(out, missing); }

    double eval(bool& missing) const override;
    virtual void expandStars(const std::vector<std::reference_wrapper<const SQLTable>>&,
                             expression::Expressions&) override;
    const eckit::sql::type::SQLType* type() const override;
    std::string tableColumnToFullname(const SQLColumn& column) const override;

    // friend std::ostream& operator<<(std::ostream& s,const BitColumnExpression& p)
    //	{ p.print(s); return s; }
    friend class SQLSelectFactory;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql::expression

#endif
