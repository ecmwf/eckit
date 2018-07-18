/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLSession.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef odb_sql_SQLSession_H
#define odb_sql_SQLSession_H

namespace eckit { class PathName; }
namespace eckit { class DataHandle; }

#include "eckit/eckit.h"

#include "eckit/sql/SQLAST.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLInsertFactory.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace eckit {
namespace sql {

class SQLOutput;
class SQLDatabase;
class SQLStatement;
class SQLTable;
class SQLOutputConfig;

class SQLSession {
public:
    SQLSession(const odb::sql::SQLOutputConfig& config, const std::string& csvDelimiter);
    virtual ~SQLSession(); 

    virtual SQLDatabase& openDatabase(const eckit::PathName&,const std::string& name = "");
    virtual void closeDatabase(const std::string& name);

    virtual void createIndex(const std::string&,const std::string&);

    virtual SQLDatabase* getDatabase(const std::string& name);

    virtual SQLSelectFactory& selectFactory();
    virtual SQLInsertFactory& insertFactory();

    virtual SQLTable* findTable(const odb::sql::Table&);

    virtual SQLTable* openDataHandle(eckit::DataHandle &);
    virtual SQLTable* openDataStream(std::istream &, const std::string &);

    virtual void statement(const SelectAST& s);
    virtual void statement(SQLStatement*) = 0;
    virtual SQLStatement* statement() = 0;
    virtual SQLOutput* defaultOutput() = 0;

    virtual SQLDatabase& currentDatabase() const;
    virtual SQLDatabase& currentDatabase(SQLDatabase*);

    virtual unsigned long long execute(SQLStatement&);

    virtual void interactive() {}

    unsigned long long lastExecuteResult() { return lastExecuteResult_; }

    bool gotSelectAST() const;
    void gotSelectAST(bool);
    const SelectAST& selectAST() const;

    std::string csvDelimiter() { return csvDelimiter_; }
    const odb::sql::SQLOutputConfig& outputConfig() { return config_; }

    static std::string readIncludeFile(const std::string&);

protected:
    void loadDefaultSchema();

private:
// No copy allowed

    SQLSession(const SQLSession&);
    SQLSession& operator=(const SQLSession&);

    static std::string schemaFile();
    static std::vector<std::string> includePaths();

    SQLDatabase* currentDatabase_;
    //std::map<int,double> params_;
    std::map<std::string,SQLDatabase*> databases_;
    SQLSelectFactory selectFactory_;
    SQLInsertFactory insertFactory_;

    SelectAST selectAST_;
    bool gotSelectAST_;
    unsigned long long lastExecuteResult_;

    const odb::sql::SQLOutputConfig config_;
    const std::string csvDelimiter_;

    friend std::ostream& operator<<(std::ostream& s, const SQLSession& p)
    {
        s << "[session@" << &p << ", currentDatabase: " << *(p.currentDatabase_) << " ]";
        return s;
    }

};

} // namespace sql
} // namespace eckit

#endif
