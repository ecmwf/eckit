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




config::Function::Function(config::Compiler &c, const string &name) : 
    Statement()
{
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
    out << "call " <<  name_ << std::endl;
}
