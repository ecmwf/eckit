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

config::Assignment::Assignment(config::Compiler &c)
{
    variable_ = c.parseIdentifier();
    c.consume('=');
    value_ = c.parseValue();
    c.consume(';');
}
config::Assignment::~Assignment()
{
}

void config::Assignment::execute(const StringDict &in, StringDict &out)
{
    out[ variable_ ] = value_;
}

void config::Assignment::print(ostream &out)
{
    out << variable_ << " = " << value_  << ";" << std::endl;
}
