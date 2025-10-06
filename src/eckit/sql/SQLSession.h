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
/// @date Dec 03

#ifndef odb_sql_SQLSession_H
#define odb_sql_SQLSession_H

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}

#include <memory>

#include "eckit/memory/OnlyMovable.h"
#include "eckit/sql/SQLSelectFactory.h"
// #include "eckit/sql/SQLInsertFactory.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLOutput;
class SQLDatabase;
class SQLStatement;
class SQLTable;
class SQLOutputConfig;

class SQLSession : private eckit::OnlyMovable {
public:

    // Constructors

    SQLSession(std::unique_ptr<SQLOutput> out, std::unique_ptr<SQLOutputConfig> config = 0,
               const std::string& csvDelimiter = ",");
    SQLSession(std::unique_ptr<SQLOutputConfig> config, const std::string& csvDelimiter = ",");
    SQLSession(std::unique_ptr<SQLOutput> out, const std::string& csvDelimiter);
    SQLSession(const std::string& csvDelimiter = ",");

    virtual ~SQLSession();

    // For sqly.y (used parsing SQL strings)

    virtual SQLSelectFactory& selectFactory();
    //    virtual SQLInsertFactory& insertFactory();

    virtual SQLTable& findTable(const std::string& name);

    //    virtual SQLTable* openDataHandle(eckit::DataHandle &);
    //    virtual SQLTable* openDataStream(std::istream &, const std::string &);

    virtual void setStatement(SQLStatement*);
    virtual SQLStatement& statement();
    virtual SQLOutput& output();

    virtual const SQLDatabase& currentDatabase() const;
    virtual SQLDatabase& currentDatabase();

    virtual unsigned long long execute(SQLStatement&);

    virtual void interactive() {}

    unsigned long long lastExecuteResult() { return lastExecuteResult_; }

    std::string csvDelimiter() { return csvDelimiter_; }
    //    const SQLOutputConfig& outputConfig() { ASSERT(config_); return *config_; }

    std::unique_ptr<SQLOutput> newFileOutput(const eckit::PathName& path);

    static std::string readIncludeFile(const std::string&);

protected:

    void loadDefaultSchema();

private:

    static std::string schemaFile();
    static std::vector<std::string> includePaths();

    SQLDatabase database_;

    // std::map<int,double> params_;
    //    std::map<std::string,SQLDatabase*> databases_;

    SQLSelectFactory selectFactory_;
    //    SQLInsertFactory insertFactory_;

    unsigned long long lastExecuteResult_;

    std::unique_ptr<SQLOutputConfig> config_;

    std::unique_ptr<SQLStatement> statement_;
    std::unique_ptr<SQLOutput> output_;
    const std::string csvDelimiter_;

    friend std::ostream& operator<<(std::ostream& s, const SQLSession& p) {
        s << "[session@" << &p << ", currentDatabase: " << p.currentDatabase() << " ]";
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
