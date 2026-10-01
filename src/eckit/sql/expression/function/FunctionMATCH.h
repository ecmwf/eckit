// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
