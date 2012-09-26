/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/config/Assignment.h"
#include "eclib/config/Block.h"
#include "eclib/config/Branch.h"

namespace config {

Block::Block(Compiler &c, bool skipBraces) : Statement()
{
    while( true )
    {
        switch( c.peek() )
        {
            case 0:
                return;
            case ']':
                return;
            case '[':
                statements_.push_back( new Branch(c) );
                break;
            default:
                statements_.push_back( new Assignment(c) );
                break;
        }
    }
}

Block::~Block()
{
    for( std::deque<Statement*>::iterator s = statements_.begin(); s != statements_.end(); ++s )
        delete *s;
}

void Block::execute(const StringDict &in, StringDict &out)
{
    for( std::deque<Statement*>::iterator s = statements_.begin(); s != statements_.end(); ++s )
        (*s)->execute(in,out);
}

void Block::print(ostream &out)
{
    for( std::deque<Statement*>::iterator s = statements_.begin(); s != statements_.end(); ++s )
        (*s)->print(out);
}

} // namespace config
