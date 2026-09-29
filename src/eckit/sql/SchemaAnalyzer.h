// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Piotr Kuchta
/// @author Simon Smart
/// date April 2009
/// date January 2019

#ifndef eckit_sql_SchemaAnalyzer_H
#define eckit_sql_SchemaAnalyzer_H

#include <set>

#include "eckit/sql/SQLTypedefs.h"
#include "eckit/sql/SchemaComponents.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SchemaAnalyzer {

public:  // methods

    SchemaAnalyzer();
    ~SchemaAnalyzer();

    void addTable(TableDef& table);

    void addBitfieldType(const std::string& name, const FieldNames& fields, const Sizes& sizes);
    //, const std::string& typeSignature);

    const BitfieldDef& getBitfieldType(const std::string& typeName) const;
    const BitfieldDef& getBitfieldTypeDefinition(const std::string& columnName) const;

    std::string generateSelectAll(const std::set<std::string>& skipTables = std::set<std::string>()) const;

    TableDefs definitions() const;

    //    bool isBitfield(const std::string& columnName) const;
    //    const BitfieldDef& getBitfieldTypeDefinition(const std::string& columnName);


    //    bool tableKnown(const std::string& name) const;
    //    const TableDef& findTable(const std::string& name) const;
    //    void skipTable(std::string tableName);
    //    Definitions generateDefinitions();
    //    std::string findColumnType(const std::string&);

private:

    BitfieldDefs bitfieldTypes_;
    TableDefs tableDefs_;

    std::map<std::string, std::string> columnTypes_;

    // std::string currentSchema_;
    // SchemaDefs schemas_;
    // TableDefs tableDefs_;
    // BitfieldDefs bitfieldTypes_;
    // std::set<std::string> tablesToSkip_;
    // std::map<std::string,std::string> columnTypes_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
