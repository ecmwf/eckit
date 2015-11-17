/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Assignment.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

Assignment::Assignment(Compiler &c, Scope& scope) :
    Statement(scope)
{
//    std::cout << "Assignment()" << std::endl;
    
    variable_ = c.parseIdentifier();
    
    char n = c.peek();
    if( n == ':' )
        c.consume(':');
    else
        c.consume('=');
    
    const bool dontBreakOnSpace = true;
    
    value_ = c.parseValue(dontBreakOnSpace);
    
    const bool include_spaces = true;

    while( true )
    {
        n = c.peek(include_spaces);
        switch( n )
        {
            case '\n':
                return;
            case '}':
                return;
            case ';':
                c.consume(';');   // accept missing ';' @ block end
                return;
            case '#':
                c.consumeComment();
                return;
            case ' ':
                c.next(include_spaces);
                break;
            default:
                throw StreamParser::Error( std::string("unexpected character after assignment : ") + n );
        }
    }
}

Assignment::~Assignment()
{
}

void Assignment::execute(const StringDict &in, StringDict &out)
{
    out[ variable_ ] = value_;
}

void Assignment::print(std::ostream&out)
{
    out << variable_ << " = " << value_  << ";" << std::endl;
}

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit
