/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
    typedef odb::sql::SQLStatement* P;
    if (gotSelectAST()) {
        gotSelectAST(false);
        statement_ = P(selectFactory().create(*this, selectAST()));
    }
    return statement_;
}

}  // namespace sql
}  // namespace eckit
