/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file FunctionMATCH.h
/// Piotr Kuchta - ECMWF October 2015

#ifndef FunctionMATCH_H
#define FunctionMATCH_H

#include <set>
#include <vector>

#include "eckit/sql/SQLAST.h"
#include "eckit/sql/expression/function/FunctionExpression.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

class FunctionMATCH : public FunctionExpression {
public:

    FunctionMATCH(const std::string&, const eckit::sql::expression::Expressions&, const eckit::sql::SelectAST&);
    FunctionMATCH(const FunctionMATCH&);
    ~FunctionMATCH();

    FunctionMATCH& operator=(const FunctionMATCH&);

    std::shared_ptr<SQLExpression> clone() const override;

    void collect(const std::vector<double>&);

private:

    size_t size_;
    const SelectAST subquery_;
    std::vector<std::vector<double> > subqueryResult_;

    const eckit::sql::type::SQLType* type() const override;
    double eval(bool& missing) const override;
    virtual void prepare(SQLSelect& sql);

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionMATCH& p)
    //	{ p.print(s); return s; }
};

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit

#endif
