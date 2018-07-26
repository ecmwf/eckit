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

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/sql/type/SQLType.h"
#include "eckit/sql/SQLTypedefs.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

//class SQLFile;
class SQLColumn;
class SQLDatabase;

class SQLTableIterator {
public:
	virtual ~SQLTableIterator() {}
	virtual void rewind() = 0;
	virtual bool next()   = 0;
};

typedef std::vector<std::string> ColumnNames;

class SQLTable : private eckit::NonCopyable {
public:
	SQLTable(SQLDatabase&,const std::string&,const std::string&);
	virtual ~SQLTable(); 

    void loadIOMAP(std::istream&);
    void addColumn(const std::string&, int, const type::SQLType&, bool, double, bool isBitfield=false, const BitfieldDef& d=BitfieldDef());

    void addLinkFrom(const SQLTable&);
	bool hasLinkFrom(const SQLTable&) const;

    void addLinkTo(const SQLTable&);
	bool hasLinkTo(const SQLTable&) const;

	bool isParentOf(const SQLTable&) const;

    virtual SQLColumn& column(const std::string&);

	//virtual bool hasColumn(const std::string&, std::string* fullName = 0, bool *hasMissingValue=0, double *missingValue=0, BitfieldDef*=0);
	virtual bool hasColumn(const std::string&, std::string* fullName = 0);//, std::string* fullName = 0, bool *hasMissingValue=0, double *missingValue=0, BitfieldDef*=0);

	unsigned long long noRows() const;

	ColumnNames columnNames() const;
	FieldNames bitColumnNames(const std::string&) const;

	const std::string& name()  const { return name_; }

	std::string fullName() const;
	SQLDatabase& owner() const { return owner_; }

    const std::string& path() const { return path_; }

	virtual void print(std::ostream& s) const;

    virtual SQLTableIterator* iterator(const std::vector<std::reference_wrapper<SQLColumn>>&) const = 0;

protected:
    std::string path_;
	std::string   name_;

	//std::map<int,SQLFile*> files_;

    std::map<std::string, FieldNames > bitColumnNames_;
    std::map<std::string, SQLColumn*>  columnsByName_;
    std::map<int, SQLColumn*>     columnsByIndex_;

    std::set<std::reference_wrapper<const SQLTable>> linksFrom_;
    std::set<std::reference_wrapper<const SQLTable>> linksTo_;

// -- Methods
	void clearColumns();
	
	// void print(std::ostream&) const; 	
	void addColumn(SQLColumn*, const std::string&, int);

	virtual SQLColumn* createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double missingValue, const BitfieldDef&) = 0;
	virtual SQLColumn* createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double missingValue) = 0;

private:

	SQLDatabase& owner_;

	friend std::ostream& operator<<(std::ostream& s,const SQLTable& p)
		{ p.print(s); return s; }

};

// helper

inline bool operator<(std::reference_wrapper<const SQLTable> lhs, std::reference_wrapper<const SQLTable> rhs) {
    return lhs.get() < rhs.get();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace eckit

#endif
