// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
