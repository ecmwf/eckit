/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// @author Simon Smart
// @date January 2019

#ifndef eckit_sql_SchemaComponents_H
#define eckit_sql_SchemaComponents_H

#include <string>
#include <vector>

#include "eckit/sql/SQLTypedefs.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class ColumnDef {

public:  // methods

    ColumnDef(const std::string& name, const std::string& type, const BitfieldDef& bitfield);
    //              const Range& range,
    //              const std::string& defaultValue);
    ColumnDef();
    ~ColumnDef();

    const std::string& name() const { return name_; }
    const std::string& type() const { return type_; }

    bool isBitfield() const { return !bitfield_.first.empty(); }
    const BitfieldDef& bitfield() const { return bitfield_; }

private:  // members

    std::string name_;
    std::string type_;
    BitfieldDef bitfield_;
};

using ColumnDefs = std::vector<ColumnDef>;

//----------------------------------------------------------------------------------------------------------------------

class TableDef {

public:  // methods

    TableDef(const std::string& name, const ColumnDefs& columns);
    ~TableDef();

    const std::string& name() const { return name_; }
    const ColumnDefs& columns() const { return columns_; }

private:  // members

    std::string name_;
    ColumnDefs columns_;
};

using TableDefs = std::vector<TableDef>;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
