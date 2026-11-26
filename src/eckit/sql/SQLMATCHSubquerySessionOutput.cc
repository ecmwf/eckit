// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/SQLMATCHSubquerySessionOutput.h"
#include "eckit/exception/Exceptions.h"
#include "odb_api/Expressions.h"
#include "odb_api/FunctionMATCH.h"

using namespace std;

namespace eckit {
namespace sql {

SQLMATCHSubquerySessionOutput::SQLMATCHSubquerySessionOutput(const SQLMATCHSubquerySessionOutput& other) :
    f_(other.f_), count_(0) {}

SQLMATCHSubquerySessionOutput::SQLMATCHSubquerySessionOutput(odb::sql::expression::function::FunctionMATCH& f) :
    f_(f), count_(0) {}

SQLMATCHSubquerySessionOutput& SQLMATCHSubquerySessionOutput::operator=(const SQLMATCHSubquerySessionOutput& other) {
    f_     = other.f_;
    count_ = other.count_;
    return *this;
}

SQLMATCHSubquerySessionOutput::~SQLMATCHSubquerySessionOutput() {}

void SQLMATCHSubquerySessionOutput::print(std::ostream& s) const {
    s << "SQLMATCHSubquerySessionOutput";
}

void SQLMATCHSubquerySessionOutput::size(int) {}

void SQLMATCHSubquerySessionOutput::reset() {}

void SQLMATCHSubquerySessionOutput::flush() {}

bool SQLMATCHSubquerySessionOutput::output(const expression::Expressions& results) {
    const size_t nCols(results.size());
    vector<double> v(nCols);
    for (size_t i(0); i < nCols; ++i) {
        bool missing(false);
        v[i] = results[i]->eval(missing);
    }
    f_.collect(v);
    ++count_;
    return true;
}

unsigned long long SQLMATCHSubquerySessionOutput::count() {
    return count_;
}

void SQLMATCHSubquerySessionOutput::prepare(SQLSelect& sql) {}

void SQLMATCHSubquerySessionOutput::cleanup(SQLSelect& sql) {}

}  // namespace sql
}  // namespace eckit
