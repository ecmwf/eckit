/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLSession.h
// Piotr Kuchta - ECMWF May 2015

#ifndef odb_api_SQLSession_H
#define odb_api_SQLSession_H

#include "eckit/sql/SQLSession.h"

namespace eckit {
namespace sql {

class SQLOutputConfig;

class SQLSession : public SQLSession {
public:
    SQLSession(const odb::sql::SQLOutputConfig&, const std::string&);
	~SQLSession(); 

	SQLStatement* statement();
private:
// No copy allowed
	SQLSession(const SQLSession&);
	SQLSession& operator=(const SQLSession&);

    SQLStatement* statement_;

// -- Overridden methods
	void statement(SQLStatement*);
	SQLOutput* defaultOutput();
};

} // namespace sql 
} // namespace eckit 

#endif
