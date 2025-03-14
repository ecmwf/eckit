/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Simon Smart
// @date Dec 03

#ifndef eckit_sql_SQLTable_H
#define eckit_sql_SQLTable_H

#include <functional>
#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/sql/SQLTypedefs.h"
#include "eckit/sql/type/SQLType.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

// class SQLFile;
class SQLColumn;
class SQLDatabase;

class SQLTableIterator {
public:

    virtual ~SQLTableIterator() {}
    virtual void rewind()                                = 0;
    virtual bool next()                                  = 0;
    virtual std::vector<size_t> columnOffsets() const    = 0;
    virtual const double* data() const                   = 0;
    virtual std::vector<size_t> doublesDataSizes() const = 0;
    virtual std::vector<char> columnsHaveMissing() const = 0;  // n.b. don't use std::vector<bool> ...
    virtual std::vector<double> missingValues() const    = 0;
};

typedef std::vector<std::string> ColumnNames;

class SQLTable : private eckit::NonCopyable {
public:

    SQLTable(SQLDatabase&, const std::string&, const std::string&);
    virtual ~SQLTable();

    void loadIOMAP(std::istream&);
    void addColumn(const std::string& name, int index, const type::SQLType& type, bool hasMissingValue,
                   double missingValue, bool isBitfield = false, const BitfieldDef& bitfieldDef = BitfieldDef());

    void addLinkFrom(const SQLTable&);
    bool hasLinkFrom(const SQLTable&) const;

    void addLinkTo(const SQLTable&);
    bool hasLinkTo(const SQLTable&) const;

    bool isParentOf(const SQLTable&) const;

    virtual const SQLColumn& column(const std::string&) const;
    void updateColumnDoublesWidth(const std::string& name, size_t nDoubles);
    void updateColumnMissingValues(const std::string& name, bool hasMissing, double missingValue);
    //    virtual SQLColumn& column(const std::string&);

    virtual bool hasColumn(const std::string& name) const;

    unsigned long long noRows() const;

    ColumnNames columnNames() const;
    FieldNames bitColumnNames(const std::string&) const;

    const std::string& name() const { return name_; }

    std::string fullName() const;
    SQLDatabase& owner() const { return owner_; }

    const std::string& path() const { return path_; }

    virtual void print(std::ostream& s) const;

    virtual SQLTableIterator* iterator(const std::vector<std::reference_wrapper<const SQLColumn>>&,
                                       std::function<void(SQLTableIterator&)> metadataUpdateCallback) const = 0;

protected:

    std::string path_;
    std::string name_;

    // std::map<int,SQLFile*> files_;

    std::map<std::string, FieldNames> bitColumnNames_;
    std::map<std::string, SQLColumn*> columnsByName_;
    std::map<int, SQLColumn*> columnsByIndex_;

    std::vector<std::unique_ptr<SQLColumn>> ownedColumns_;

    std::set<std::reference_wrapper<const SQLTable>> linksFrom_;
    std::set<std::reference_wrapper<const SQLTable>> linksTo_;

    // -- Methods
    void clearColumns();

    // void print(std::ostream&) const;
    void addColumn(SQLColumn*, const std::string&, int);

    virtual SQLColumn* createSQLColumn(const type::SQLType& type, const std::string& name, size_t index,
                                       bool hasMissingValue, double missingValue, const BitfieldDef& d = BitfieldDef());

private:

    SQLDatabase& owner_;

    friend std::ostream& operator<<(std::ostream& s, const SQLTable& p) {
        p.print(s);
        return s;
    }
};

// helper

inline bool operator<(std::reference_wrapper<const SQLTable> lhs, std::reference_wrapper<const SQLTable> rhs) {
    return &lhs.get() < &rhs.get();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
