/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odb_api/SelectIterator.h"
#include "eckit/sql/SQLIteratorOutput.h"
#include "eckit/sql/SQLIteratorSession.h"

namespace eckit {
namespace sql {

SQLIteratorSession::SQLIteratorSession(SelectIterator &it, SQLSession& s)
: SQLSession(s.outputConfig(), s.csvDelimiter()),
  statement_(0),
  iterator_(it),
  session_(s)
{}

SQLIteratorSession::~SQLIteratorSession() {}

SQLOutput* SQLIteratorSession::defaultOutput() { return new SQLIteratorOutput<>(iterator_); }

void SQLIteratorSession::statement(odb::sql::SQLStatement *sql)
{
	ASSERT(sql);
	statement_ = sql;
}

SQLStatement* SQLIteratorSession::statement()
{
    typedef odb::sql::SQLStatement* P;

    if (session_.gotSelectAST())
    {
        session_.gotSelectAST(false);
        statement_ = P(session_.selectFactory().create(*this, session_.selectAST()));
    }
    return statement_;
}


SQLDatabase& SQLIteratorSession::openDatabase(const eckit::PathName& p,const std::string& name) { return session_.openDatabase(p, name); }
void SQLIteratorSession::closeDatabase(const std::string& name) { session_.closeDatabase(name); }

void SQLIteratorSession::createIndex(const std::string& a, const std::string& b) { session_.createIndex(a,b); }

SQLDatabase* SQLIteratorSession::getDatabase(const std::string& name) { return session_.getDatabase(name); }

SQLSelectFactory& SQLIteratorSession::selectFactory() { return session_.selectFactory(); }
SQLInsertFactory& SQLIteratorSession::insertFactory() { return session_.insertFactory(); }

SQLTable* SQLIteratorSession::findTable(const odb::sql::Table& t) { return session_.findTable(t); }

SQLTable* SQLIteratorSession::openDataHandle(eckit::DataHandle &h) { return session_.openDataHandle(h); }
SQLTable* SQLIteratorSession::openDataStream(std::istream &is, const std::string &s) { return session_.openDataStream(is, s); }

void SQLIteratorSession::statement(const SelectAST& s) { session_.statement(s); }
//void SQLIteratorSession::statement(SQLStatement* s) { session_.statement(s); }
//SQLStatement* SQLIteratorSession::statement() { return session_.statement(); }
//SQLOutput* SQLIteratorSession::defaultOutput() { return session_.defaultOutput(); }

SQLDatabase& SQLIteratorSession::currentDatabase() const { return session_.currentDatabase(); }
SQLDatabase& SQLIteratorSession::currentDatabase(SQLDatabase* s) { return session_.currentDatabase(s); }

unsigned long long SQLIteratorSession::execute(SQLStatement& s) { return session_.execute(s); }

} // namespace sql 
} // namespace eckit 
