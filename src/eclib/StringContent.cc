/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/StringContent.h"
#include "eclib/JSON.h"

ClassSpec StringContent::classSpec_ = {&Content::classSpec(),"StringContent",};
Reanimator<StringContent> StringContent::reanimator_;


StringContent::StringContent(const string& s): 
	value_(s) 
{ 
}

StringContent::StringContent(const char* s): 
	value_(s)
{ 
}

StringContent::StringContent(Stream& s):
	Content(s)
{
	s >> value_;
}

void StringContent::encode(Stream& s) const
{
	Content::encode(s);
	s << value_;
}

StringContent::~StringContent()
{
}

void StringContent::print(ostream& s) const
{
	s << value_;
}

void StringContent::json(JSON& s) const
{
    s << value_;
}

int StringContent::compare(const Content& other) const
{
	return -other.compareString(*this);
}

int StringContent::compareString(const StringContent& other) const
{
	return ::strcmp(value_.c_str(),other.value_.c_str());
}

void StringContent::value(string& s) const 
{ 
	s = value_; 
}

void StringContent::value(bool& b) const 
{ 
    if( value_ == "true" || value_ == "on" || value_ == "yes" || value_ == "1" )
    {
        b = true;
    }
    else
    {
            if( value_ == "false" || value_ == "off" || value_ == "no" || value_ == "0" )
                b = false;
            else
                Content::value(b);
    }
}

Content* StringContent::add(const Content& other) const
{
	return other.addString(*this);
}

Content* StringContent::addString(const StringContent& other) const
{
	return new StringContent(other.value_ + value_);
}

Content* StringContent::sub(const Content& other) const
{
    return other.subString(*this);
}

Content* StringContent::mul(const Content& other) const
{
    return other.mulString(*this);
}

Content* StringContent::div(const Content& other) const
{
    return other.divString(*this);
}

Content* StringContent::mod(const Content& other) const
{
    return other.modString(*this);
}
