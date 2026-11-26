// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/sql/SelectOneTable.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

static bool nullBool = false;

SelectOneTable::SelectOneTable(const SQLTable* table) :
    table_(table),
    offset_(0, nullBool),
    length_(0, nullBool),
    column_{nullptr},
    table1_{nullptr},
    table2_{nullptr},
    order_(0) {}

SelectOneTable::~SelectOneTable() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql
