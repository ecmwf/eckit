/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Sep 2012

#include "eckit/os/BackTrace.h"
#include "eckit/parser/StreamParser.h"
#include "eckit/utils/Translator.h"

#include "eckit/log/Log.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StreamParser::StreamParser(std::istream &in, bool comments, char comment) :
    line_(0),
    in_(in),
    comments_(comments),
    comment_(comment)
{
}

char StreamParser::peek(bool spaces)
{
    for(;;)
    {
        char c = in_.peek();

        if(in_.eof())
            return 0;

        if(comments_ && c == comment_)
        {
            while(c != '\n' && !in_.eof()) {
                in_.get(c);
                if(c == '\n') { line_++; }
            }
            if(in_.eof()) {
                return 0;
            }
            return peek(spaces);
        }

        if(spaces || !isspace(c))
        {
//            std::cout << "peek(" << c << ")" << std::endl;
            return c;
        }
        else {
//            std::cout << "skip(" << c << ")" << std::endl;
            in_.get(c);
            if(c == '\n') { line_++; }
        }
    }
}

char StreamParser::next(bool spaces)
{
    char c;
    for(;;)
    {
        in_.get(c);
        if(in_.eof())
            throw StreamParser::Error(std::string("StreamParser::next reached eof"));

        if(c == '\n') { line_++; }

        if(comments_ && c == comment_)
        {
            while(c != '\n' && !in_.eof()) {
                in_.get(c);
                if(c == '\n') { line_++; }
            }
            if(in_.eof()) {
                throw StreamParser::Error(std::string("StreamParser::next reached eof"));
            }
            return next(spaces);
        }

        if(spaces || !isspace(c))
        {
//            std::cout << "next(" << c << ")" << std::endl;
            return c;
        }
    }

}

void StreamParser::consume(char c)
{
    char n = next();
    if(c != n)
        throw StreamParser::Error(std::string("StreamParser::consume expecting '") + c + "', got '" + n + "'", line_ + 1);
}

void StreamParser::consume(const char* p)
{
    while(*p) consume(*p++);
}


StreamParser::Error::Error(const std::string &what, size_t line) : Exception(what)
{
    if(line) {
        std::ostringstream oss;
        oss << "Line: " << line << " " << what;
        reason(oss.str());
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

