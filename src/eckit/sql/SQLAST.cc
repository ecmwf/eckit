/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLAST.h"

using namespace std;

namespace eckit {
namespace sql {

ColumnDef::ColumnDef()
{}

ColumnDef::ColumnDef(const std::string& name, const std::string& type, const Range& range,
        const std::string& defaultValue)
  : name_(name),
    type_(type),
    range_(range),
    defaultValue_(defaultValue),
    hasDefaultValue_(!defaultValue.empty())
{}

ConstraintDef::ConstraintDef()
  : type_(NONE),
    name_(""),
    columns_(0),
    relatedTable_(""),
    relatedColumns_(0)
{}

ConstraintDef::ConstraintDef(const std::string& name, const std::vector<std::string>& primaryKey)
  : type_(PRIMARY_KEY),
    name_(name),
    columns_(primaryKey),
    relatedTable_(""),
    relatedColumns_(0)
{}

ConstraintDef::ConstraintDef(const std::string& name, const std::vector<std::string>& foreignKey,
        const std::string& relatedTable, const std::vector<std::string>& relatedColumns)
  : type_(FOREIGN_KEY),
    name_(name),
    columns_(foreignKey),
    relatedTable_(relatedTable),
    relatedColumns_(relatedColumns)
{}

TableDef::TableDef()
{}

TableDef::TableDef(const std::string& name, 
                   const ColumnDefs& columns,
                   const ConstraintDefs& constraints, 
                   const std::vector<std::string>& parents,
                   const std::string& location)
  : name_(name),
    columns_(columns),
    constraints_(constraints),
    parents_(parents),
    location_(location)
{}

SchemaDef::SchemaDef()
{}

SchemaDef::SchemaDef(const TableDefs& tables)
  : tables_(tables)
{}

Definitions::Definitions()
{}

Definitions::Definitions(const SchemaDefs& schemas, const TableDefs& tables)
  : schemas_(schemas),
    tables_(tables)
{}

} // namespace sql
} // namespace eckit
