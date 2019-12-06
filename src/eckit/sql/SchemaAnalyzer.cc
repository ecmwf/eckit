/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SchemaAnalyzer.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/StringTools.h"

using namespace eckit;

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SchemaAnalyzer::SchemaAnalyzer() {}

SchemaAnalyzer::~SchemaAnalyzer() {}

void SchemaAnalyzer::addTable(TableDef& table) {

    tableDefs_.push_back(table);

    for (auto& col : table.columns()) {
        std::string fullname   = col.name() + "@" + table.name();
        columnTypes_[fullname] = col.type();
        // Log::debug<LibOdc>() << "SchemaAnalyzer::addTable(): columnTypes_[" << fullname << "] = "
        //                      << columnTypes_[fullname] << std::endl;
    }
}

void SchemaAnalyzer::addBitfieldType(const std::string& name, const FieldNames& fields, const Sizes& sizes) {
    bitfieldTypes_[name] = make_pair(fields, sizes);
}

const BitfieldDef& SchemaAnalyzer::getBitfieldType(const std::string& typeName) const {

    auto it = bitfieldTypes_.find(typeName);
    if (it != bitfieldTypes_.end()) {
        return it->second;
    }

    static const BitfieldDef nullBitfieldDef;
    return nullBitfieldDef;
}

const BitfieldDef& SchemaAnalyzer::getBitfieldTypeDefinition(const std::string& columnName) const {

    auto it = columnTypes_.find(columnName);
    ASSERT(it != columnTypes_.end());

    return getBitfieldType(it->second);
}

std::string SchemaAnalyzer::generateSelectAll(const std::set<std::string>& skipTables) const {

    std::string from;
    std::string selectlist;

    if (tableDefs_.empty()) {
        return "";
    }

    for (const auto& table : tableDefs_) {

        // Tables to skip?

        if (skipTables.find(table.name()) != skipTables.end()) {
            continue;
        }

        if (!from.empty())
            from += ", ";
        from += table.name();

        for (const auto& col : table.columns()) {

            std::string fullname = col.name() + "@" + table.name();

            if (col.type() == "@LINK") {
                Log::info() << "SchemaAnalyzer::generateSelectAll(): Skipping " << fullname << std::endl;
            }
            else {
                if (!selectlist.empty())
                    selectlist += ", ";
                selectlist += fullname;
            }
        }
        selectlist += "\n";
    }
    return "\nSELECT\n" + selectlist + "\n FROM\n" + from;
}

TableDefs SchemaAnalyzer::definitions() const {
    return tableDefs_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit


#if 0
SchemaAnalyzer::SchemaAnalyzer()
{
//desc, hdr, sat, reo3, satob, rtovs, rtovs_slev, rtovs_mlev, rtovs_pred, atovs, atovs_pred, scatt, ssmi, ssmi_slev, ssmi_mlev, body, errstat, update, rtovs_body, ssmi_body, scatt_body
	skipTable("poolmask");
	skipTable("bufr");
	skipTable("ddrs");
	skipTable("index");
	skipTable("rtovs");
	skipTable("rtovs_slev");
	skipTable("rtovs_mlev");
	skipTable("rtovs_pred");
	skipTable("rtovs_body");
	skipTable("ssmi");
	skipTable("ssmi_body");
	skipTable("ssmi_slev");
	skipTable("ssmi_pred");
	skipTable("ssmi_mlev");
	skipTable("atovs");
	skipTable("atovs_pred");
	skipTable("scatt");
	skipTable("scatt_body");
	skipTable("reo3");
	skipTable("sat");
	skipTable("satob");
	skipTable("update");
	skipTable("");
	skipTable("");
} 

SchemaAnalyzer::~SchemaAnalyzer() {}

void SchemaAnalyzer::beginSchema(const std::string& name)
{
    if (!currentSchema_.empty())
    {
        std::string message = "Cannot create new schema '" + name
            + "' - current schema '" + currentSchema_ + "' not finalized";
        throw eckit::UserError(message);
    }

    std::pair<SchemaDefs::iterator, bool> result;
    result = schemas_.insert(make_pair(name, SchemaDef()));

    if (result.second == false)
    {
        std::string message = "Schema '" + name + "' already defined";
        throw eckit::UserError(message);
    }

    currentSchema_ = name;
}

void SchemaAnalyzer::endSchema()
{
    currentSchema_.clear();
}

void SchemaAnalyzer::addTable(TableDef& table) 
{
    std::string schemaName = "";

    if (StringTool::isInQuotes(table.name())) 
        table.name(StringTool::unQuote(table.name()));
    else {
        size_t pos = table.name().find(".");
        if (pos != std::string::npos)
        {
            schemaName = table.name().substr(0, pos);
            table.name(table.name().substr(pos + 1));
        }
    }

	ColumnDefs& columns (table.columns());
	for (ColumnDefs::iterator it = columns.begin(); it != columns.end(); ++it) {
        ColumnDef& column(*it);
        column.name(column.name() + "@" + table.name());
        columnTypes_[column.name()] = column.type();

        if (isBitfield(column.name()))
            column.bitfieldDef(getBitfieldTypeDefinition(column.name()));
	}

    for (int i = 0, n = table.parents().size(); i < n; i++) {
        TableDefs::const_iterator it(tableDefs_.find(table.parents()[i]));

        if (it == tableDefs_.end())
            throw eckit::UserError(std::string("Could not find definition of parent table '")
                     + table.parents()[i] + "' inherited by table '" + table.name() + "'");

        const TableDef& parent (it->second);
        if(! parent.parents().empty()) throw UserError("More than 1-level inheritance not supported");

        for (ColumnDefs::const_iterator c (parent.columns().begin()); c != parent.columns().end(); ++c)
            table.columns().push_back(*c);
    }

    if (currentSchema_.empty() && schemaName.empty()) {
        std::pair<TableDefs::iterator, bool> result (tableDefs_.insert(std::pair<std::string, TableDef>(table.name(), table)));
        if (result.second == false)
            throw eckit::UserError(std::string ("Table '") + table.name() + "' already defined");
    } else {
        if (schemaName.empty())
            schemaName = currentSchema_;

        SchemaDefs::iterator it (schemas_.find(schemaName));
        if (it == schemas_.end())
            throw eckit::UserError(std::string("Referenced schema '") + schemaName + "' not defined '");

        SchemaDef& schema (it->second);
        TableDefs& tables (schema.tables());

        std::pair<TableDefs::iterator, bool> result (tables.insert(std::pair<std::string, TableDef>(table.name(), table)));
        if (result.second == false) 
            throw eckit::UserError(std::string ("Table '") + table.name() + "' already defined in '" + schemaName + "' schema");
    }
}

void SchemaAnalyzer::skipTable(std::string tableName)
{
	tablesToSkip_.insert(tableName);
}

std::string SchemaAnalyzer::generateSELECT() const
{
	std::string from = "";
	std::string selectList = "";
	if (tableDefs_.size() == 0)
		return "";

	for (TableDefs::const_iterator t = tableDefs_.begin(); t != tableDefs_.end(); ++t)
	{
		TableDef tableDef = t->second;
		std::string tableName = tableDef.name();

		if (tablesToSkip_.find(tableName) != tablesToSkip_.end())
			continue;

		from += tableName + ", ";
		ColumnDefs columnDefs = tableDef.columns();
		
		for (ColumnDefs::const_iterator i = columnDefs.begin(); i != columnDefs.end(); i++)
		{
			const std::string typeName = i->type();
			if (typeName == "@LINK") {
				Log::info() << "SchemaAnalyzer::generateSELECT: Skipping " << i->name() << std::endl;
				continue;
			}
			selectList += i->name() + ", ";
		}
		selectList += "\n";
	}
	return "\nSELECT\n" + selectList + "\n FROM\n" + from;
}

Definitions SchemaAnalyzer::generateDefinitions()
{
    return Definitions(schemas_, tableDefs_);
}

void SchemaAnalyzer::addBitfieldType(const std::string& name, const FieldNames& fields, const Sizes& sizes, const std::string& typeSignature)
{
	bitfieldTypes_[name] = make_pair(fields, sizes);
}

bool SchemaAnalyzer::isBitfield(const std::string& columnName) const
{
    for (std::map<std::string,std::string>::const_iterator it(columnTypes_.begin()); it != columnTypes_.end(); ++it)
    {
        if (it->first == columnName)
        {
            std::string columnType (it->second);
            for (std::map<std::string, BitfieldDef>::const_iterator it(bitfieldTypes_.begin()); it != bitfieldTypes_.end(); ++it)
                if (it->first == columnType)
                    return true;
        }
    }
    return false;
}

const BitfieldDef& SchemaAnalyzer::getBitfieldTypeDefinition(const std::string& columnName) 
{
	ASSERT(isBitfield(columnName));
	std::string columnType = columnTypes_.find(columnName)->second;
	return bitfieldTypes_[columnType];
}

void SchemaAnalyzer::updateBitfieldsDefs(MetaData &md, std::map<std::string,std::string> & truenames) const
{
	for (size_t i = 0; i < md.size(); i++)
	{
		Column &c (*md[i]);
		if (c.type() == BITFIELD)
			c.bitfieldDef(const_cast<SchemaAnalyzer*>(this)->getBitfieldTypeDefinition(truenames[c.name()]));
	}
}

bool SchemaAnalyzer::tableKnown(const std::string& name) const
{
    return tableDefs_.find(name) != tableDefs_.end();
}

const TableDef& SchemaAnalyzer::findTable(const std::string& name) const
{
    for (TableDefs::const_iterator it(tableDefs_.begin()); it != tableDefs_.end(); ++it)
        if (it->first == name)
            return it->second;
    throw eckit::UserError(std::string("Table '" + name + "' not found"));
}

std::string SchemaAnalyzer::findColumnType(const std::string& columnName)
{
    std::vector<std::string> ps (StringTools::split("@", columnName));
    if (ps.size() != 2)
        throw eckit::UserError(std::string("Expected fully qualified column name (<name>@<table>), got: " + columnName));

    std::string name (ps[0]), table (ps[1]);
    const TableDef& tableDef (findTable(table));
    ColumnDefs columnDefs (tableDef.columns());
    for (ColumnDefs::const_iterator it (columnDefs.begin()); it != columnDefs.end(); ++it)
        if (it->name() == columnName)
            return it->type();
    throw eckit::UserError(std::string("Column " + columnName + " not found"));
}

#endif
