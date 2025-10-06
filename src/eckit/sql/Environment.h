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

#include <memory>

#include "eckit/sql/SQLTable.h"
#include "eckit/sql/SelectOneTable.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLTableIterator;

struct Environment {

    Environment(SortedTables::iterator it) : tableIterator(it) {}
    ~Environment() {}

    const SelectOneTable& table() const { return **tableIterator; }

    // n.b. ODB-357 and performance issues.

    SortedTables::iterator tableIterator;

    std::unique_ptr<SQLTableIterator> cursor;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
