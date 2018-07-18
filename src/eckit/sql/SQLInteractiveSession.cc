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

#include "eckit/sql/SQLInteractiveSession.h"
#include "eckit/sql/SQLSimpleOutput.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace eckit {
namespace sql {

SQLInteractiveSession::SQLInteractiveSession(std::ostream &out)
: SQLSession(odb::sql::SQLOutputConfig::defaultConfig(), ","),
  out_(out),
  sql_(0)
{
    loadDefaultSchema();
}

SQLInteractiveSession::~SQLInteractiveSession()
{}

SQLOutput* SQLInteractiveSession::defaultOutput()
{
	return new SQLSimpleOutput(out_);
}

void SQLInteractiveSession::statement(SQLStatement *sql)
{
	ASSERT(sql);
    sql_ = sql;
}

SQLStatement *SQLInteractiveSession::statement()
{
    return sql_;
}

void SQLInteractiveSession::interactive()
{
    typedef odb::sql::SQLStatement* P;
    if (gotSelectAST())
    {
        gotSelectAST(false);
        sql_ = P(selectFactory().create(*this, selectAST()));
    }
    
    if (sql_)
    {
        execute(*sql_);
        delete sql_;
        sql_ = 0;
    }
}

} // namespace sql
} // namespace eckit
