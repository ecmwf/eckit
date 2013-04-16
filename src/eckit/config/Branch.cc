/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Block.h"
#include "eclib/config/Branch.h"
#include "eclib/config/Condition.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

Branch::Branch(Compiler &c, Scope& scope) :
    Statement(scope)
{
//    std::cout << "Branch()" << std::endl;
    
    if_.reset( new Condition(c) );
    then_.reset( new Block(c, new Scope(&scope)) );
    if( c.peek() == '|' )
    {
        c.consume("||");
        else_.reset( new Block(c, new Scope(&scope)) );
    }
     else
        else_.reset();
}

Branch::~Branch()
{
}

void Branch::execute(const StringDict &in, StringDict &out)
{
    if( if_->eval(in,out) )
    {
        then_->execute(in,out);
    }
    else
        if( hasElse() )
            else_->execute(in,out);
}

void Branch::print(ostream &out)
{
    if_->print(out);
    then_->print(out);
    if( hasElse() )
        else_->print(out);
}

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit
