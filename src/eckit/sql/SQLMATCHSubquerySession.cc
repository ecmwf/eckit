// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/SQLMATCHSubquerySession.h"
#include "eckit/sql/SQLMATCHSubquerySessionOutput.h"
#include "odb_api/FunctionMATCH.h"

namespace eckit {
namespace sql {

SQLMATCHSubquerySession::SQLMATCHSubquerySession(expression::function::FunctionMATCH& f) :
    SQLSession(odb::sql::SQLOutputConfig::defaultConfig(), ","), statement_(0), f_(f) {
    loadDefaultSchema();
}

SQLMATCHSubquerySession::~SQLMATCHSubquerySession() {}

SQLOutput* SQLMATCHSubquerySession::defaultOutput() {
    return new SQLMATCHSubquerySessionOutput(f_);
}

void SQLMATCHSubquerySession::statement(odb::sql::SQLStatement* sql) {
    ASSERT(sql);
    statement_ = sql;
}

SQLStatement* SQLMATCHSubquerySession::statement() {
    using P = odb::sql::SQLStatement*;
    if (gotSelectAST()) {
        gotSelectAST(false);
        statement_ = P(selectFactory().create(*this, selectAST()));
    }
    return statement_;
}

}  // namespace sql
}  // namespace eckit
