/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLParser.h
// Baudouin Raoult - ECMWF Mar 98
// Piotr Kuchta - ECMWF Mar 2012

#ifndef eckit_sql_SQLParser_H
#define eckit_sql_SQLParser_H

#include <stack>

#include "SQLOutputConfig.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLDatabase;
class SQLSession;

class SyntaxError : public eckit::Exception {
    using eckit::Exception::Exception;
};

struct ParseFrame {
    ParseFrame(const std::string& sql, const std::string& yypath);

    std::string inputString_;
    std::string yypath_;
    char* inputText_;
    char* inputEnd_;
};

class SQLParser {
public:

    static int line();

    //    static void parseString(SQLSession&, const std::string&, eckit::DataHandle*, SQLOutputConfig, bool
    //    resetSession = true); static void parseString(SQLSession&, const std::string&, std::istream*, SQLOutputConfig,
    //    const std::string& cvsDelimiter);
    static void parseString(SQLSession&, const std::string&);
    //    static void parseString(SQLSession&, const std::string&, SQLDatabase&, SQLOutputConfig);

    static std::stack<ParseFrame> frames_;

private:

    static void parseStringInternal(SQLSession&, const std::string&);

    static std::string cleanUpSQLText(const std::string&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
