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
#include "eckit/io/FileHandle.h"

#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/parser/StringTools.h"
#include <libgen.h>

using namespace eckit;

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------


SQLSession::SQLSession(const SQLOutputConfig& config, const std::string& csvDelimiter) :
    selectFactory_(*this),
//  insertFactory_(),
  lastExecuteResult_(),
  config_(config),
  csvDelimiter_(csvDelimiter)
{
    database_.open();
}

SQLSession::~SQLSession() {}

SQLSelectFactory& SQLSession::selectFactory() { return selectFactory_; }
//SQLInsertFactory& SQLSession::insertFactory() { return insertFactory_; }


unsigned long long SQLSession::execute(SQLStatement& sql)
{
    Timer timer("Execute");

    unsigned long long n = sql.execute();
    return lastExecuteResult_ = n;
}

const SQLDatabase& SQLSession::currentDatabase() const {
    return database_;
}

SQLDatabase& SQLSession::currentDatabase() {
    return database_;
}

SQLTable& SQLSession::findTable(const std::string& database, const std::string& name) {
    // Multiple database support in session not yet implemented
    NOTIMP;
}

SQLTable& SQLSession::findTable(const std::string& name) {
    return currentDatabase().table(name);
}

/*
SQLTable* SQLSession::openDataStream(std::istream &is, const std::string& delimiter)
{
    return currentDatabase().openDataStream(is, delimiter);
}

SQLTable* SQLSession::openDataHandle(DataHandle &dh)
{
    return currentDatabase().openDataHandle(dh);
}
*/


void SQLSession::loadDefaultSchema()
{
    std::string schemaPathName (schemaFile());
    if (schemaPathName.empty())
        return;

    Log::debug() << "Loading schema " << schemaPathName << std::endl;
    
    FileHandle dh(schemaPathName);
    size_t sz = dh.openForRead();
    std::string schema(sz, ' ');
    ASSERT(dh.read(&schema[0], sz) == sz);
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

    static std::string pathName (eckit::StringTools::unQuote(Resource<std::string>("$ODB_API_SCHEMA_PATH", pn)));
    return pathName;
}

std::string SQLSession::readIncludeFile(const std::string& fileName)
{
    std::vector<std::string> dirs (includePaths());
    Log::debug() << "read include: " << fileName << std::endl;

    for (size_t i(0); i < dirs.size(); ++i)
    {
        PathName pathName (dirs[i] + fileName);
        Log::debug() << "Looking for include file " << fileName << " in " << dirs[i] << std::endl;
        if (!pathName.exists())
            continue;

        FileHandle dh(pathName);
        size_t sz = dh.openForRead();
        std::string readBuf(sz, ' ');
        ASSERT(dh.read(&readBuf[0], sz) == sz);
        return readBuf;
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

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace eckit
