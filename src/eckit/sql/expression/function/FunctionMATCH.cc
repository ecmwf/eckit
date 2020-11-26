/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"

#include "eckit/sql/SQLMATCHSubquerySession.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/expression/function/FunctionEQ.h"
#include "eckit/sql/expression/function/FunctionMATCH.h"

#include <algorithm>
#include <vector>

namespace eckit {
namespace sql {
namespace expression {
namespace function {

using namespace eckit;
using namespace std;

FunctionMATCH::FunctionMATCH(const std::string& name, const expression::Expressions& args, const SelectAST& selectAST) :
    FunctionExpression(name, args), size_(args.size()), subquery_(selectAST), subqueryResult_() {}

FunctionMATCH::FunctionMATCH(const FunctionMATCH& other) :
    FunctionExpression(other.name_, other.args_),
    size_(other.args_.size()),
    subquery_(other.subquery_),
    subqueryResult_(other.subqueryResult_) {}

FunctionMATCH::~FunctionMATCH() {}

void FunctionMATCH::prepare(SQLSelect& sql) {
    FunctionExpression::prepare(sql);

    SQLMATCHSubquerySession session(*this);
    SQLSelect* select(session.selectFactory().create(session, subquery_));
    session.execute(dynamic_cast<SQLStatement&>(*select));

    std::stable_sort(subqueryResult_.begin(), subqueryResult_.end());
}

FunctionMATCH& FunctionMATCH::operator=(eckit::sql::expression::function::FunctionMATCH const&) {
    // TODO:
    NOTIMP;
    return *this;
}

void FunctionMATCH::collect(const std::vector<double>& v) {
    subqueryResult_.push_back(v);
}

const type::SQLType* FunctionMATCH::type() const {
    return &type::SQLType::lookup("real");
}  // TODO: bool?

std::shared_ptr<SQLExpression> FunctionMATCH::clone() const {
    return std::make_shared<FunctionMATCH>(*this);
}

double FunctionMATCH::eval(bool& missing) const {
    std::vector<double> vs(size_);
    for (size_t i(0); i < size_; ++i) {
        bool missing(false);
        vs[i] = args_[i]->eval(missing);
    }
    return std::binary_search(subqueryResult_.begin(), subqueryResult_.end(), vs);
}

}  // namespace function
}  // namespace expression
}  // namespace sql
}  // namespace eckit
