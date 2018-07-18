/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Timer.h"
#include "eckit/config/Resource.h"

#include "odb_api/ODADatabase.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLNonInteractiveSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "odb_api/StringTool.h"
#include <libgen.h>

using namespace eckit;

namespace eckit {
namespace sql {

std::string defaultDB = "default";

SQLSession::SQLSession(const odb::sql::SQLOutputConfig& config, const std::string& csvDelimiter)
: currentDatabase_(0),
  databases_(),
  selectFactory_(config, csvDelimiter),
  insertFactory_(),
  selectAST_(),
  gotSelectAST_(false),
  lastExecuteResult_(),
  config_(config),
  csvDelimiter_(csvDelimiter)
{
    currentDatabase_ = new ODADatabase(".", defaultDB); 
    currentDatabase_->open();
    databases_[defaultDB] = currentDatabase_;
}

SQLSession::~SQLSession()
{
    for(std::map<std::string,SQLDatabase*>::iterator j = databases_.begin(); j != databases_.end(); ++j)
        delete (*j).second;
}

SQLSelectFactory& SQLSession::selectFactory() { return selectFactory_; }
SQLInsertFactory& SQLSession::insertFactory() { return insertFactory_; }

SQLDatabase& SQLSession::currentDatabase(SQLDatabase *db)
{
    delete currentDatabase_;
    currentDatabase_ = db;
    currentDatabase_->open();
    databases_[defaultDB] = currentDatabase_;
    return *currentDatabase_;
}

SQLDatabase& SQLSession::openDatabase(const PathName& path,const std::string& name)
{
    std::map<std::string,SQLDatabase*>::iterator j = databases_.find(name);
    if(j != databases_.end())
    {
        SQLDatabase* db = (*j).second;
        db->close();
        delete db;
    }

    currentDatabase_ = new ODADatabase(path,name);
    currentDatabase_->open();

    databases_[name] = currentDatabase_;
    return *currentDatabase_;
}

void SQLSession::closeDatabase(const std::string& name)
{
    // TODO
}

unsigned long long SQLSession::execute(SQLStatement& sql)
{
    Timer timer("Execute");
    ASSERT(currentDatabase_);	

    unsigned long long n = sql.execute();
    return lastExecuteResult_ = n;
}

SQLDatabase& SQLSession::currentDatabase() const
{
    ASSERT(currentDatabase_);	
    return *currentDatabase_;
}
/*
double SQLSession::getParameter(int which) const
{
    std::map<int,double>::const_iterator j = params_.find(which);
    if(j == params_.end())
        throw eckit::UserError("Undefined parameter");
    return (*j).second;
}

void SQLSession::setParameter(int which,double value)
{
    params_[which] = value;
}
*/

SQLDatabase* SQLSession::getDatabase(const std::string& name)
{
    std::map<std::string,SQLDatabase*>::iterator j = databases_.find(name);
    if(j == databases_.end())
        throw eckit::UserError("Cannot find database", name);
    return (*j).second;
}

SQLTable* SQLSession::findTable(const odb::sql::Table& t)
{
    if (t.database.size())
        return getDatabase(t.database)->table(t);
    else
    {
        if (! currentDatabase_) 
            throw eckit::UserError("No current database", t.name);

        return currentDatabase_->table(t);
    }
}

SQLTable* SQLSession::openDataStream(std::istream &is, const std::string& delimiter)
{
    ASSERT(currentDatabase_);
    return currentDatabase_->openDataStream(is, delimiter);
}

SQLTable* SQLSession::openDataHandle(DataHandle &dh)
{
	ASSERT(currentDatabase_);
	return currentDatabase_->openDataHandle(dh);
}


void SQLSession::createIndex(const std::string& column,const std::string& table)
{
    ASSERT(currentDatabase_);
#if 0
	currentDatabase_->table(table)->column(column)->createIndex();
#endif
}

void SQLSession::statement(const SelectAST& s) 
{ 
    selectAST_ = s;
    gotSelectAST_ = true;
}

const SelectAST& SQLSession::selectAST() const { return selectAST_; }

bool SQLSession::gotSelectAST() const { return gotSelectAST_; }
void SQLSession::gotSelectAST(bool b) { gotSelectAST_ = b; }


void SQLSession::loadDefaultSchema()
{
    std::string schemaPathName (schemaFile());
    if (schemaPathName.empty())
        return;

    Log::debug() << "Loading schema " << schemaPathName << std::endl;
    
    std::string schema (StringTool::readFile(schemaPathName));
    SQLOutputConfig config (selectFactory().config());
    SQLParser parser;
    // TODO: update include path
    parser.parseString(*this, schema, static_cast<DataHandle*>(0), config);
}

std::string SQLSession::schemaFile()
{
    const char* pn = 
#ifdef ODB_API_SCHEMA_PATH
# define STR_VALUE(s)                #s
# define STRING(s)                   STR_VALUE(s)
# define ODB_API_SCHEMA_PATH_STRING  STRING(ODB_API_SCHEMA_PATH) 
    STRING(ODB_API_SCHEMA_PATH_STRING)
#endif
    "";

    static std::string pathName (StringTool::unQuote(Resource<std::string>("$ODB_API_SCHEMA_PATH", pn)));
    return pathName;
}

std::string SQLSession::readIncludeFile(const std::string& fileName)
{
    std::vector<std::string> dirs (includePaths());
    Log::debug() << "read include: " << fileName << std::endl;

    for (size_t i(0); i < dirs.size(); ++i)
    {
        std::string pathName (dirs[i] + fileName);
        Log::debug() << "Looking for include file " << fileName << " in " << dirs[i] << std::endl;
        if (! PathName(pathName).exists())
            continue;
        return StringTool::readFile(pathName);
    }
    throw eckit::UserError(std::string("Include file '") + fileName + "' not found");
}

std::vector<std::string> SQLSession::includePaths()
{
    std::vector<std::string> r;
    std::string s (schemaFile());
    char a [s.size() + 1];
    strncpy(a, s.c_str(), s.size() + 1);
    const std::string dir (dirname(const_cast<char *>(a)));

    r.push_back("");
    r.push_back(dir + "/");
    r.push_back("./");
    return r;
}

} // namespace sql
} // namespace eckit
