/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <libgen.h>
#include <cstring>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/StringTools.h"

#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/SQLTableFactory.h"

using namespace eckit;

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------


SQLSession::SQLSession(std::unique_ptr<SQLOutput> out, std::unique_ptr<SQLOutputConfig> config,
                       const std::string& csvDelimiter) :
    selectFactory_(*this),
    lastExecuteResult_(),
    config_(config ? std::move(config) : std::unique_ptr<SQLOutputConfig>(new SQLOutputConfig())),
    output_(std::move(out)),
    csvDelimiter_(csvDelimiter) {

    ASSERT(output_ || config_);
    database_.open();
}

SQLSession::SQLSession(std::unique_ptr<SQLOutputConfig> config, const std::string& csvDelimiter) :
    SQLSession(0, std::move(config), csvDelimiter) {}

SQLSession::SQLSession(std::unique_ptr<SQLOutput> out, const std::string& csvDelimiter) :
    SQLSession(std::move(out), 0, csvDelimiter) {}

SQLSession::SQLSession(const std::string& csvDelimiter) :
    SQLSession(0, 0, csvDelimiter) {}

SQLSession::~SQLSession() {}

SQLSelectFactory& SQLSession::selectFactory() {
    return selectFactory_;
}
// SQLInsertFactory& SQLSession::insertFactory() { return insertFactory_; }


unsigned long long SQLSession::execute(SQLStatement& sql) {
    Timer timer("Execute");

    unsigned long long n      = sql.execute();
    return lastExecuteResult_ = n;
}

std::unique_ptr<SQLOutput> SQLSession::newFileOutput(const eckit::PathName& path) {
    return std::unique_ptr<SQLOutput>(config_->buildOutput(path));
}

const SQLDatabase& SQLSession::currentDatabase() const {
    return database_;
}

SQLDatabase& SQLSession::currentDatabase() {
    return database_;
}

void SQLSession::setStatement(SQLStatement* s) {
    statement_.reset(s);
}

SQLStatement& SQLSession::statement() {
    ASSERT(statement_);
    return *statement_;
}

SQLOutput& SQLSession::output() {

    ASSERT(output_ || config_);

    if (!output_)
        output_.reset(config_->buildOutput());

    return *output_;
}

SQLTable& SQLSession::findTable(const std::string& name) {
    if (!currentDatabase().hasTable(name)) {
        Log::debug<LibEcKit>() << "No table named \"" << name << "\" found. Looking in table factory" << std::endl;
        currentDatabase().addTable(SQLTableFactory::instance().build(currentDatabase(), name));
    }
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


void SQLSession::loadDefaultSchema() {
    std::string schemaPathName(schemaFile());
    if (schemaPathName.empty())
        return;

    Log::debug<LibEcKit>() << "Loading schema " << schemaPathName << std::endl;

    FileHandle dh(schemaPathName);
    size_t sz = dh.openForRead();
    std::string schema(sz, ' ');
    ASSERT(size_t(dh.read(&schema[0], sz)) == sz);
    SQLParser parser;
    // TODO: update include path
    parser.parseString(*this, schema);
}

std::string SQLSession::schemaFile() {
    // TODO; This resource is a really bad way of doing this...
    static std::string pathName(eckit::StringTools::unQuote(Resource<std::string>("$ECKIT_SQL_SCHEMA_PATH", "")));
    return pathName;
}

std::string SQLSession::readIncludeFile(const std::string& fileName) {
    std::vector<std::string> dirs(includePaths());
    Log::debug<LibEcKit>() << "read include: " << fileName << std::endl;

    for (size_t i(0); i < dirs.size(); ++i) {
        PathName pathName(dirs[i] + fileName);
        Log::debug<LibEcKit>() << "Looking for include file " << fileName << " in " << dirs[i] << std::endl;
        if (!pathName.exists())
            continue;

        FileHandle dh(pathName);
        size_t sz = dh.openForRead();
        std::string readBuf(sz, ' ');
        ASSERT(size_t(dh.read(&readBuf[0], sz)) == sz);
        return readBuf;
    }
    throw eckit::UserError(std::string("Include file '") + fileName + "' not found");
}

std::vector<std::string> SQLSession::includePaths() {
    std::vector<std::string> r;
    r.push_back("");
    r.push_back(PathName(schemaFile()).dirName().asString() + "/");
    r.push_back("./");
    return r;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
