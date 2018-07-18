/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLNonInteractiveSession.h"
#include "eckit/sql/SQLSimpleOutput.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace eckit {
namespace sql {

SQLNonInteractiveSession::SQLNonInteractiveSession(const odb::sql::SQLOutputConfig& config, const std::string& csvDelimiter)
: SQLSession(config, csvDelimiter),
  statement_(0)
{
    loadDefaultSchema();
}

SQLNonInteractiveSession::~SQLNonInteractiveSession()
{}

SQLOutput* SQLNonInteractiveSession::defaultOutput()
{
    return new SQLSimpleOutput(std::cout);
}

void SQLNonInteractiveSession::statement(SQLStatement *sql)
{
    delete statement_;
    statement_ = sql;
}

SQLStatement * SQLNonInteractiveSession::statement()
{
    typedef odb::sql::SQLStatement* P;
    if (gotSelectAST())
    {
        gotSelectAST(false);
        statement_ = P(selectFactory().create(*this, selectAST()));
    }
    return statement_;
}

} // namespace sql
} // namespace eckit
