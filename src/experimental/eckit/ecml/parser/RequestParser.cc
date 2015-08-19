/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "experimental/eckit/ecml/parser/RequestParser.h"

#include "eckit/thread/AutoLock.h"
#include "eckit/log/Log.h"
#include "eckit/thread/Mutex.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/exception/Exceptions.h"

#include <fstream>
#include <stdexcept> 
#include <sstream>

namespace eckit {

struct RequestParserMutex : public eckit::AutoLock<eckit::Mutex> {
    RequestParserMutex();
private:
    static eckit::Mutex mutex_;
};

static void reset_parser(FILE*,bool);
static void do_parse_request();
static void do_parse_request_in_string(const char *);

class RequestParseError : public eckit::Exception
{
public:
    RequestParseError(const char*);
};

struct RequestParserResult { static Cell* result_; };

Request RequestParser::parse(const std::string& s, bool debug)
{
    RequestParserMutex mutex;
    std::stringstream ss;
    // two zeroes are required by yy_scan_buffer (request__scan_buffer)
    // I add this extra '\0' here in case we move to yy_scan_buffer at a point.
    ss << s << "\0"; 

    do_parse_request_in_string(ss.str().c_str()); 

    Request result (RequestParserResult::result_);
    RequestParserResult::result_ = 0;

    return result;
}

Request RequestParser::parseFile(const char* path, bool debug)
{
    RequestParserMutex mutex;
    return parseFileUnprotected(path, debug);
}

typedef RequestParser ClientRequestParser;

Request RequestParser::parseFileUnprotected(const char* path, bool debug)
{
    RequestParserResult::result_ = 0;

    FILE* in (::fopen(path, "r"));
    if(!in) throw eckit::CantOpenFile(path);

    reset_parser(in, debug);
    try { do_parse_request(); }
    catch (RequestParseError e) {
        fclose(in);
        throw eckit::UserError(e.what());
    }
    fclose(in);

    Request result (RequestParserResult::result_);
    RequestParserResult::result_ = 0;

    //result->showGraph("AST");
    return result;
}

Request RequestParserResult::result_ = 0;

eckit::Mutex RequestParserMutex::mutex_;
RequestParserMutex::RequestParserMutex() : eckit::AutoLock<eckit::Mutex>(mutex_) {}

RequestParseError::RequestParseError(const char* s)
: Exception(s)
{}

namespace RequestYacc {

    extern "C" 
    {
        int request_wrap(void);
        void request_error(const char* msg);
    }

#include "experimental/eckit/ecml/requesty.c"

    extern "C" 
    {
        int request_wrap(void) { return 1; }
        void request_error(const char* msg)
        { 
            std::stringstream ss;
            ss << std::string(msg) << " line " << request_lineno;
            throw RequestParseError(ss.str().c_str());
        }
    }
}

void reset_parser(FILE* in, bool debug)
{
    RequestYacc::request_lineno = 0;
    RequestYacc::request_in     = in;
    RequestYacc::request_debug  = debug;
}

void do_parse_request()
{
    RequestYacc::request_parse();
}

void do_parse_request_in_string(const char *s)
{
    RequestYacc::YY_BUFFER_STATE buffer;
    try { 
        buffer = RequestYacc::request__scan_string(s);
        RequestYacc::request_parse();
    }
    catch (RequestParseError e)
    {
        RequestYacc::request__delete_buffer(buffer);
        throw UserError(e.what());
    }
    request__delete_buffer(buffer);
}

} // namespace eckit 
