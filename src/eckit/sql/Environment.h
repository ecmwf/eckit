/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_sql_Environment_H
#define eckit_sql_Environment_H

#include "eckit/sql/SelectOneTable.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLTableIterator;

struct Environment {
	Environment (const SortedTables::iterator);
	~Environment ();

    // these functions **must** be inline for performance reasons
    // see ODB-357
    SelectOneTable*   table()  { return table_; }
    void              table(SelectOneTable* t)  { table_ = t; }

    // these functions **must** be inline for performance reasons
    // see ODB-357
    SQLTableIterator* cursor() { return cursor_; }
    void              cursor(SQLTableIterator* c) { cursor_ = c; }

	void print(std::ostream& s) const;

	const SortedTables::iterator tablesIterator_;

private:

    SelectOneTable* table_;
	SQLTableIterator* cursor_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace eckit

#endif
