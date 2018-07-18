/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLIteratorSession.h
// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLIteratorSession_H
#define SQLIteratorSession_H

#include "eckit/sql/SQLSession.h"

namespace eckit {

class SelectIterator;

namespace sql {

class SQLIteratorSession : public SQLSession {
public:
	SQLIteratorSession(odb::SelectIterator &, SQLSession&);
	~SQLIteratorSession(); 

// -- Overridden methods

	SQLDatabase& openDatabase(const eckit::PathName&,const std::string& name = "");
	void closeDatabase(const std::string& name);

	void createIndex(const std::string&,const std::string&);

	SQLDatabase* getDatabase(const std::string& name);

    SQLSelectFactory& selectFactory();
    SQLInsertFactory& insertFactory();

	SQLTable* findTable(const odb::sql::Table&);

	SQLTable* openDataHandle(eckit::DataHandle &);
    SQLTable* openDataStream(std::istream &, const std::string &);

	void statement(const SelectAST& s);
	void statement(SQLStatement*);
	SQLStatement* statement();
	SQLOutput* defaultOutput();

	SQLDatabase& currentDatabase() const;
	SQLDatabase& currentDatabase(SQLDatabase*);

    unsigned long long execute(SQLStatement&);

private:
// No copy allowed
	SQLIteratorSession(const SQLIteratorSession&);
	SQLIteratorSession& operator=(const SQLIteratorSession&);

	SQLStatement* statement_;
	odb::SelectIterator& iterator_;
    SQLSession& session_;
};

} // namespace sql
} // namespace eckit

#endif
