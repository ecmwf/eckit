// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date   Dec 2003

#ifndef eckit_sql_SelectOneTable_H
#define eckit_sql_SelectOneTable_H

#include "eckit/sql/expression/SQLExpressions.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLColumn;

// Forward declarations
class SQLTableIterator;

struct SelectOneTable {
    SelectOneTable(const SQLTable* table = nullptr);
    ~SelectOneTable();

    const SQLTable* table_;

    // Information about the data to be retrieved.

    std::vector<std::reference_wrapper<const SQLColumn>> fetch_;
    std::vector<std::pair<const double*, bool>*> values_;
    //    std::vector<std::pair<double*,bool&>>    values_;

    // How do we find the data inside the allocated buffer in SQLSelect?
    //    std::vector<size_t> fetchSizeDoubles_;

    Expressions check_;
    Expressions index_;


    // For links
    std::pair<const double*, bool&> offset_;
    std::pair<const double*, bool&> length_;
    const SQLColumn* column_;  // Reference column

    // For checking/debugging
    const SQLTable* table1_;
    const SQLTable* table2_;

    // For index

    // For sorting
    int order_;
};

using SortedTables = std::vector<SelectOneTable*>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
