// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/SchemaComponents.h"

using namespace eckit;

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

ColumnDef::ColumnDef(const std::string& name, const std::string& type, const BitfieldDef& bitfield) :
    //                     const Range &range,
    //                     const std::string& defaultValue) {}
    name_(name), type_(type), bitfield_(bitfield) {}

ColumnDef::ColumnDef() {}

ColumnDef::~ColumnDef() {}

TableDef::TableDef(const std::string& name, const ColumnDefs& columns) : name_(name), columns_(columns) {}

TableDef::~TableDef() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql
