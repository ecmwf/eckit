/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stack>

#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/SchemaComponents.h"
#include "eckit/sql/expression/BitColumnExpression.h"
#include "eckit/sql/expression/ColumnExpression.h"
#include "eckit/sql/expression/NumberExpression.h"
#include "eckit/sql/expression/ParameterExpression.h"
#include "eckit/sql/expression/SQLExpression.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/expression/StringExpression.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/sql/type/SQLBitfield.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/StringTools.h"

using namespace eckit;

#define YYDEBUG 1

namespace SQLYacc {

typedef void* eckit_sql_scan_t;

void eckit_sql_error(eckit_sql_scan_t, eckit::sql::SQLSession*, const char* msg);

using namespace eckit;
using namespace eckit::sql;
using namespace eckit::sql::expression;
using namespace eckit::sql::expression::function;

#include "eckit/sql/sqly.c"

void eckit_sql_error(eckit_sql_scan_t scanner, eckit::sql::SQLSession*, const char* msg) {
    std::stringstream os;

    struct eckit_sql_guts_t* eckit_sql_g = (struct eckit_sql_guts_t*)scanner;
    // internally we count the lines from 0...
    int lineNumber(1 + eckit_sql_g->eckit_sql_lineno_r);

    os << "SQL " << (msg ? msg : "syntax error") << ", line " << lineNumber  // << " of " << yypath;
       << ". See https://software.ecmwf.int/wiki/display/ODBAPI/SQL\n";
    throw SyntaxError(os.str());
}

}  // namespace SQLYacc

SQLYacc::Stack& includeStack(void* eckit_sql_scanner) {
    SQLYacc::Stack* stack(
        static_cast<SQLYacc::Stack*>(((struct SQLYacc::eckit_sql_guts_t*)eckit_sql_scanner)->eckit_sql_extra_r));
    ASSERT(stack);
    return *stack;
}

extern "C" int eckit_sql_wrap(void* scanner) {
    return includeStack(scanner).pop(scanner);
}

namespace eckit {
namespace sql {

// void SQLParser::parseString(SQLSession& session, const std::string& s, std::istream* is)
//{
//
//    session.selectFactory().implicitFromTableSourceStream(is);
//    NOTIMP;
////    session.selectFactory().config(cfg);
////    session.selectFactory().csvDelimiter(csvDelimiter);
//
//    parseStringInternal(session, s);
//
////    SQLYacc::eckit_sql_lex_init(&scanner); // TODO: handle unwind
//}
//
// void SQLParser::parseString(SQLSession& session, const std::string& s, DataHandle* dh)
//{
//
//    session.selectFactory().implicitFromTableSource(dh);
//    NOTIMP;
////    session.selectFactory().config(cfg);
//
//    parseStringInternal(session, s);
//}

// void SQLParser::parseString(SQLSession& session,const std::string& s, SQLDatabase& db, SQLOutputConfig cfg)
//{

//    session.selectFactory().database(&db);
//    session.selectFactory().config(cfg);

//    parseStringInternal(session, s);
//}

void SQLParser::parseString(SQLSession& session, const std::string& s) {

    SQLYacc::eckit_sql_scan_t scanner;
    SQLYacc::include_stack stack;
    SQLYacc::eckit_sql_lex_init_extra(&stack, &scanner);

    stack.push(cleanUpSQLText(s), "", (SQLYacc::YY_BUFFER_STATE)scanner, (SQLYacc::eckit_sql_scan_t)scanner);
    SQLYacc::eckit_sql_parse(scanner, &session);
    SQLYacc::eckit_sql_lex_destroy(scanner);
}

std::string SQLParser::cleanUpSQLText(const std::string& sql) {

    if (sql.empty())
        return sql;

    std::string s(sql);

    s = eckit::StringTools::trim(s);
    s = eckit::StringTools::unQuote(s);
    s = eckit::StringTools::trim(s);

    if (!s.empty() && *s.rbegin() != ';') {
        s.push_back(';');
    }

    return s;
}

}  // namespace sql
}  // namespace eckit
