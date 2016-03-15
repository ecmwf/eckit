/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Function.h"
#include "eckit/config/Scope.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

Function::Function(config::Compiler& c, config::Scope& scope) : 
    Statement(scope)
{
//    std::cout << "Function()" << std::endl;    
    
    // '[' consumed outside    
    c.consume("function");
    name_ = c.parseIdentifier();
    c.consume(']');
    body_.reset( new config::Block(c,new config::Scope(scope)) );
    scope.function(name_,this);
}

Function::~Function()
{
}

void config::Function::execute(const StringDict &in, StringDict &out)
{
    body_->execute(in,out);
}

void config::Function::print(std::ostream&out)
{
    out << "[ function " <<  name_  << "]" << std::endl;
    body_->print(out);
}

//-----------------------------------------------------------------------------

FunctionCall::FunctionCall(config::Compiler& c, config::Scope &scope) :
    Statement(scope)
{   
    //    std::cout << "FunctionCall()" << std::endl;    
    
    // '[' consumed outside        
    c.consume("call");
    name_ = c.parseIdentifier();
    c.consume(']');
    
    scope.existsFunction(name_);
}

FunctionCall::~FunctionCall()
{
}

void FunctionCall::execute(const StringDict &in, StringDict &out)
{
    scope().function(name_).execute(in,out);
}

void FunctionCall::print(std::ostream&out)
{
    out << "[ call " <<  name_  << "]" << std::endl;    
}

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit
