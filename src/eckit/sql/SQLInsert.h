/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLInsert.h
// Piotr Kuchta - ECMWF July 2016

#ifndef SQLInsert_H
#define SQLInsert_H

#include "odb_api/Expressions.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/SQLAST.h"

namespace eckit {
namespace sql {
// Forward declarations

class SQLDatabase;

class SQLInsert : public SQLStatement {
public:
	SQLInsert(const InsertAST&);
	virtual ~SQLInsert(); 

    virtual unsigned long long execute();
	virtual expression::Expressions output() const;


    const Table& table() const { return table_; }
    const std::vector<std::string>& columns() const { return columns_; }
    const std::vector<std::string>& values() const { return values_; }

protected:
	virtual void print(std::ostream&) const; 	

private:
// No copy allowed
	SQLInsert(const SQLInsert&);
	SQLInsert& operator=(const SQLInsert&);

	friend std::ostream& operator<<(std::ostream& s,const SQLInsert& p)
		{ p.print(s); return s; }

    Table table_;
    std::vector<std::string> columns_;
    std::vector<std::string> values_;
};

} // namespace sql 
} // namespace eckit 

#endif
