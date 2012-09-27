/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/config/Function.h"
#include "eclib/config/Scope.h"

//-----------------------------------------------------------------------------

config::Function::Function(config::Compiler& c, config::Scope& scope) : 
    Statement(scope)
{
    // '[' consumed outside    
    c.consume("function");
    name_ = c.parseIdentifier();
    c.consume(']');
    body_.reset( new config::Block(c,new config::Scope(scope)) );
    scope.function(name_,this);
}

config::Function::~Function()
{
}

void config::Function::execute(const StringDict &in, StringDict &out)
{
    body_->execute(in,out);
}

void config::Function::print(ostream &out)
{
    out << "[ function " <<  name_  << "]" << std::endl;
    body_->print(out);
}

//-----------------------------------------------------------------------------

config::FunctionCall::FunctionCall(config::Compiler& c, config::Scope &scope) :
    Statement(scope)
{
    // '[' consumed outside        
    c.consume("call");
    name_ = c.parseIdentifier();
    c.consume(']');
    
    scope.existsFunction(name_);
}

config::FunctionCall::~FunctionCall()
{
}

void config::FunctionCall::execute(const StringDict &in, StringDict &out)
{
    scope().function(name_).execute(in,out);
}

void config::FunctionCall::print(ostream &out)
{
    out << "[ call " <<  name_  << "]" << std::endl;    
}
