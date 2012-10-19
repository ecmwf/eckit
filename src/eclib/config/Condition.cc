/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/BadValue.h"
#include "eclib/Tokenizer.h"

#include "eclib/config/Condition.h"

config::Condition::Condition( config::Compiler& c )
{
    // '[' consumed outside    
    c.consume("if"); // branch keyword
    
    parseSentence(c);
    
    while(c.peek()=='&')
    {
        c.consume("&&");
        parseSentence(c);
    }
    
    c.consume(']');
}

config::Condition::~Condition()
{
}


bool config::Condition::eval( const StringDict& in, StringDict& out )
{
    bool ret( true );
    for( size_t i = 0; i < sentences_.size(); ++i )
    {
        StringDict::const_iterator key = in.find( sentences_[i].first );
        if( key == in.end() )
            return false;
//            throw BadValue( "key not found in input dictionary : " + sentences_[i].first );
        
        const string& left = key->second;

        StringList& rightValues = sentences_[i].second;
        bool match(false);
        for( size_t j = 0; j < rightValues.size(); ++j )
        {
            if( left == rightValues[j] )
                match = true;
        }
        ret &= match;
    }    
    return ret;
}

void config::Condition::print( std::ostream& out )
{
    out << "[ if ";
    for( size_t i = 0; i < sentences_.size(); ++i )
    {
        out << sentences_[i].first << " = ";
        StringList& rightValues = sentences_[i].second;
        out << rightValues[0];            
        for( size_t j = 1; j < rightValues.size(); ++j )
        {
            out << " | " << rightValues[j];
        }
        if( i != sentences_.size() -1 )
            out << " & ";
        else
            out << "]";
    }
    out << std::endl;
}

void config::Condition::parseSentence(Compiler& c)
{
    if( c.peek() == ']' ) // for empty conditions
        return;
    
    string variable = c.parseIdentifier();
    c.consume('=');
    sentences_.push_back( make_pair( variable, c.parseCondition() ));
}

