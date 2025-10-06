/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Simon Smart
/// @author Baudouin Raoult
/// @date Dec 2003

#ifndef eckit_sql_SQLDatabase_H
#define eckit_sql_SQLDatabase_H

#include <map>
#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/sql/SQLTable.h"
#include "eckit/sql/SchemaAnalyzer.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

// Forward declarations

class SQLStatement;
namespace expression {
class SQLExpression;
}

typedef std::map<std::string, std::set<std::string>> Links;
typedef std::map<std::string, std::shared_ptr<expression::SQLExpression>> Variables;


//----------------------------------------------------------------------------------------------------------------------

class SQLDatabase {
public:

    SQLDatabase(const std::string& name = "default");
    virtual ~SQLDatabase();

    SQLDatabase(const SQLDatabase&)            = delete;
    SQLDatabase& operator=(const SQLDatabase&) = delete;

    SQLDatabase(SQLDatabase&&)            = default;
    SQLDatabase& operator=(SQLDatabase&&) = default;

    // -- Methods
    void open();
    void close();

    /// Access SQLTables in DB
    bool hasTable(const std::string& name) const;
    SQLTable& table(const std::string& name);
    SQLTable& defaultTable();
    std::vector<std::reference_wrapper<SQLTable>> implicitTables();

    //    SQLTable& openDataHandle(eckit::DataHandle&);
    //    SQLTable& openDataStream(std::istream&, const std::string& delimiter);

    void addTable(SQLTable* table);
    void addImplicitTable(SQLTable* table);

    void setLinks(const Links&);
    void setLinks() { setLinks(links_); }

    void addLinks(const Links& ls) { links_.insert(ls.begin(), ls.end()); }
    Links& links() { return links_; }

    const std::string& name() const { return name_; }

    std::shared_ptr<expression::SQLExpression> getVariable(const std::string&) const;
    void setVariable(const std::string&, std::shared_ptr<expression::SQLExpression>);

    Variables& variables() { return variables_; }

    SchemaAnalyzer& schemaAnalyzer() { return schemaAnalyzer_; }

    void setIncludePath(const std::string& includePath);
    const std::vector<eckit::PathName>& includePath() const { return includePath_; }

protected:

    Links links_;
    std::map<std::string, std::unique_ptr<SQLTable>> tablesByName_;
    std::vector<std::unique_ptr<SQLTable>> implicitTables_;

    std::vector<eckit::PathName> includePath_;

    Variables variables_;
    std::string name_;
    SchemaAnalyzer schemaAnalyzer_;

private:

    // No copy allowed

    void loadIOMAP();
    void loadDD();
    void loadFLAGS();

    // -- Friends
    friend std::ostream& operator<<(std::ostream& s, const SQLDatabase& p) {
        s << "[SQLDatabase@" << &p << " tables: ";
        for (const auto& it : p.tablesByName_) {
            s << it.first << ",";
        }

        s << "]";
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif  // eckit_sql_SQLDatabase_H
