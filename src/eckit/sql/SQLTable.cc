/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLTable.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLBitColumn.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {
namespace sql {

SQLTable::SQLTable(SQLDatabase& owner, const std::string& path, const std::string& name) :
    path_(path), name_(name), owner_(owner) {
    Log::debug<LibEcKit>() << "new SQLTable[path=" << path_ << ",name=" << name << "]" << std::endl;
}

SQLTable::~SQLTable() {
    clearColumns();
}

void SQLTable::clearColumns() {}

std::vector<std::string> SQLTable::columnNames() const {
    std::vector<std::string> results;
    for (std::map<int, SQLColumn*>::const_iterator j = columnsByIndex_.begin(); j != columnsByIndex_.end(); ++j)
        results.push_back((*j).second->name());
    return results;
}

FieldNames SQLTable::bitColumnNames(const std::string& name) const {
    typedef std::map<std::string, FieldNames>::const_iterator I;
    I i = bitColumnNames_.find(name);
    if (i != bitColumnNames_.end())
        return (*i).second;

    ASSERT("name not found" && name.find("@") == std::string::npos);

    std::string columnName;
    FieldNames fieldNames;
    size_t counter = 0;
    for (I i = bitColumnNames_.begin(); i != bitColumnNames_.end(); ++i) {
        if (i->first.find(name + "@") == 0) {
            columnName = i->first;
            fieldNames = i->second;
            ++counter;
        }
    }
    if (counter == 0)
        throw eckit::UserError(std::string("Column '") + name + "' not found.");
    if (counter != 1)
        throw eckit::UserError(std::string("Ambiguous column name: '") + name + "'");

    return fieldNames;
}


// void SQLTable::addColumn(const std::string& name, int index, const type::SQLType& type, const FieldNames& bitmap)
void SQLTable::addColumn(const std::string& name, int index, const type::SQLType& type, bool hasMissingValue,
                         double missingValue, bool isBitfield, const BitfieldDef& bitfieldDef) {
    const FieldNames& bitmap = bitfieldDef.first;
    std::unique_ptr<SQLColumn> col(isBitfield
                                       ? createSQLColumn(type, name, index, hasMissingValue, missingValue, bitfieldDef)
                                       : createSQLColumn(type, name, index, hasMissingValue, missingValue));

    // ownedColumns_ describes columns for deletion. Once we sort out the bitfield/SQLBitColumn
    // ambiguity in this file, ownedColumns_ should hopefully become superfluous, and
    // columnsByName_ can do the owning.

    columnsByName_[name]   = col.get();
    columnsByIndex_[index] = col.get();

    bitColumnNames_[name] = bitmap;

    std::vector<std::string> tokens;
    Tokenizer("@")(name, tokens);

    ASSERT(tokens.size() == 1 || tokens.size() == 2);

    // TODO: clean up, probably no need to do this parsing as we have the whole bitfieldDef now
    std::string tableName  = (tokens.size() == 2) ? tokens[1] : "";
    std::string columnName = tokens[0];

    for (FieldNames::const_iterator j = bitmap.begin(); j != bitmap.end(); ++j) {
        std::string fieldName = *j;
        std::string n         = columnName + "." + fieldName;
        if (!tableName.empty())
            n += "@" + tableName;
        columnsByName_[n] = col.get();

        // Log::info() << "SQLTable::addColumn: columnsByName_[" << n << "] = " << *col << std::endl;
    }

    ownedColumns_.push_back(std::move(col));
}

void SQLTable::addColumn(SQLColumn* col, const std::string& name, int index) {
    columnsByName_[name]   = col;
    columnsByIndex_[index] = col;
}

SQLColumn* SQLTable::createSQLColumn(const type::SQLType& type, const std::string& name, size_t index,
                                     bool hasMissingValue, double missingValue, const BitfieldDef& defs) {
    ASSERT(type.size() % 8 == 0);
    return new SQLColumn(type, *this, name, index, hasMissingValue, missingValue, defs);
}

bool SQLTable::hasColumn(const std::string& name) const {
    std::map<std::string, SQLColumn*>::const_iterator j = columnsByName_.find(name);
    return j != columnsByName_.end();
}

unsigned long long SQLTable::noRows() const {
    std::map<std::string, SQLColumn*>::const_iterator j = columnsByName_.begin();
    if (j != columnsByName_.end())
        return (*j).second->noRows();
    return 0;
}


const SQLColumn& SQLTable::column(const std::string& name) const {
    std::map<std::string, SQLColumn*>::const_iterator j = columnsByName_.find(name);
    if (j != columnsByName_.end())
        return *(j->second);

    std::vector<std::string> v;
    Tokenizer(".")(name, v);

    // *** HACK alert ***
    // TODO: this does not seem to be the correct place to construct SQLBitColumns

    // If we are referring to a bit column, construct it here. This is essentially a
    // "caching" trick, so it is sorta-ok that we mutate state despite this function
    // being logically const...

    // TODO: The combination of this and SQLTable::addColumn() construction seems duplicative
    // something is not right, and one of these should be removed.

    if (v.size() > 1) {
        if (hasColumn(v[0])) {
            const SQLColumn& col  = column(v[0]);
            SQLTable* mutableThis = const_cast<SQLTable*>(this);
            std::unique_ptr<SQLColumn> newCol(new SQLBitColumn(col, v[1]));
            SQLColumn* pnewCol                = newCol.get();
            mutableThis->columnsByName_[name] = pnewCol;
            mutableThis->ownedColumns_.push_back(std::move(newCol));
            return *pnewCol;
        }
    }

    // *** End HACK ****

    throw eckit::UserError("Column not found", name);
}

void SQLTable::updateColumnDoublesWidth(const std::string& name, size_t nDoubles) {

    std::map<std::string, SQLColumn*>::const_iterator j = columnsByName_.find(name);
    if (j == columnsByName_.end())
        throw eckit::UserError("Column not found", name);

    if (j->second->type().getKind() == type::SQLType::stringType) {
        j->second->updateType(type::SQLType::lookup("string", nDoubles));
    }
    else {
        ASSERT(nDoubles == 1);
    }
}

void SQLTable::updateColumnMissingValues(const std::string& name, bool hasMissing, double missingValue) {
    std::map<std::string, SQLColumn*>::const_iterator j = columnsByName_.find(name);
    if (j == columnsByName_.end())
        throw eckit::UserError("Column not found", name);

    j->second->hasMissingValue(hasMissing);
    j->second->missingValue(missingValue);
}

void SQLTable::addLinkFrom(const SQLTable& from) {
    linksFrom_.insert(from);
}

bool SQLTable::hasLinkFrom(const SQLTable& from) const {
    //    return linksFrom_.find(from) != linksFrom_.end();
    return false;
}

void SQLTable::addLinkTo(const SQLTable& to) {
    linksTo_.insert(to);
}

bool SQLTable::hasLinkTo(const SQLTable& to) const {
    //    return linksTo_.find(to) != linksTo_.end();
    return false;
}

bool SQLTable::isParentOf(const SQLTable& other) const {
    if (hasLinkTo(other))
        return true;

    for (const auto& l : linksTo_) {
        if (l.get().isParentOf(other))
            return true;
    }

    return false;
}

std::string SQLTable::fullName() const {
    return owner_.name() + "." + name_;
}

void SQLTable::print(std::ostream& s) const {
    s << "CREATE TABLE " << fullName() << " AS (" << std::endl;
    for (std::map<int, SQLColumn*>::const_iterator j = columnsByIndex_.begin(); j != columnsByIndex_.end(); ++j) {
        SQLColumn* c = j->second;
        s << "	" << c->name() << " " << c->type() << "," << std::endl;
    }
    s << ")" << std::endl;
}

}  // namespace sql
}  // namespace eckit
