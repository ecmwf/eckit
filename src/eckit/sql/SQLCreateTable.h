/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLCreateTable.h
/// 
/// Piotr Kuchta - ECMWF Jan 09
/// 

#ifndef SQLCreateTable_H
#define SQLCreateTable_H

#include "odb_api/Expressions.h"

namespace eckit {
namespace sql {

// Forward declarations
class SQLCreateTable {
public:

// -- Exceptions
	// None

// -- Contructors

	SQLCreateTable(std::string tableName, ColumnDefs &cols);

// -- Destructor

	virtual ~SQLCreateTable(); 

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	virtual unsigned long long execute();

	// TODO: This method makes no sense for statements not producing
	// result sets, like e.g. CREATE TABLE, so I think it could be
	// removed from SQLStatement. Perhaps SQLStatement should have two
	// abstract subclasses, one for statements which produce result sets
	// (rows of data), one for those producing update count
	// (like UPDATE, DELETE, CREATE TABLE|INDEX|...), similarly as in JDBC.
	virtual Expressions output() const { return Expressions(); }

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

	virtual void print(std::ostream&) const; 	
protected:

// -- Members
	// None

// -- Methods
	
	//virtual void print(std::ostream&) const; 	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	SQLCreateTable(const SQLCreateTable&);
	SQLCreateTable& operator=(const SQLCreateTable&);

// -- Members
	std::string tableName_;
	ColumnDefs cols_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const SQLCreateTable& p)
		{ p.print(s); return s; }

};

} // namespace sql 
} // namespace eckit 
#endif
