/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SchemaComponents.h"

using namespace eckit;

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

ColumnDef::ColumnDef(const std::string& name, const std::string& type, const BitfieldDef& bitfield) :
    //                     const Range &range,
    //                     const std::string& defaultValue) {}
    name_(name),
    type_(type),
    bitfield_(bitfield) {}

ColumnDef::ColumnDef() {}

ColumnDef::~ColumnDef() {}

TableDef::TableDef(const std::string& name, const ColumnDefs& columns) :
    name_(name), columns_(columns) {}

TableDef::~TableDef() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
