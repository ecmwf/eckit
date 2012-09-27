/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ctype.h>

#include "eclib/Log.h"
#include "eclib/Tokenizer.h"

#include "eclib/config/Compiler.h"

namespace config {

static bool isUnderscore( char c ) { return c == '_'; }

Compiler::Compiler( istream& in ) : StreamParser(in) {}

Compiler::~Compiler() {}

string Compiler::parseIdentifier()
{
    string s;
    char c = peek();
    
    if( isdigit(c) )
        throw StreamParser::Error(string("Compiler::parseIdentifier -- 1st char in identifier cannot be digit"));        
    
    while( isIdentifier(c) )
    {
        s += next();
        c = peek();
    }
    
    if( s.empty() )
        throw StreamParser::Error(string("Compiler::parseIdentifier -- empty identifier"));
    return s;
}

string Compiler::parseValue()
{    
    string s;
    char c = peek();
    
    bool quoted = false;
    char quote;
    if( c == '\'' || c == '\"')
    {
        quote = c;
        quoted = true;
        consume(quote);
        c = peek();
    }
    
    while( c && isIdentifier(c) || (quoted && ( c != quote )) )
    {
        s += next(quoted);
        c = peek(quoted);
    }
    
    if( c == quote )
        consume(quote);

    return s;
}

StringList Compiler::parseCondition()
{
    StringList s;

    s.push_back( parseValue() );
    
    char c = peek();
    while( c == '|' )
    {
       consume("||");
       s.push_back( parseValue() );
       c = peek();
    }

    return s;
}

bool Compiler::isIdentifier(char c)
{
    return ( isalnum(c) || isUnderscore(c) );
}

} // namespace config
