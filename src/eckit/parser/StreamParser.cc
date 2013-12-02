/*
 * (C) Copyright 1996-2013 ECMWF.
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
#include "eckit/parser/Translator.h"

#include "eckit/log/Log.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


StreamParser::StreamParser(std::istream &in) : in_(in)
{
}

char StreamParser::peek(bool spaces)
{ 
    char c;
    for(;;)
    {
        c = in_.peek();
        if(in_.eof())
            return 0;
        if(spaces || !isspace(c))
        {
            //cout << "peek(" << c << ")" << std::endl;
            return c;
        }
        else {
            //cout << "skip(" << c << ")" << std::endl;
            in_.get(c);
        }
    }
}

char StreamParser::next(bool spaces)
{
    char c;
    for(;;)
    {
        if(in_.eof())
            throw StreamParser::Error(std::string("StreamParser::next reached eof"));
        in_.get(c);
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
        throw StreamParser::Error(std::string("StreamParser::consume expecting '") + c + "', got '" + n + "'");
}

void StreamParser::consume(const char* p)
{
    while(*p) consume(*p++);
}


StreamParser::Error::Error(const std::string &what) : what_(what) 
{
    Log::info() << "=== StreamParser::Error -- Backtrace ===" << std::endl;    
    Log::info() << BackTrace::dump() << std::endl;    
}

//-----------------------------------------------------------------------------

} // namespace eckit

