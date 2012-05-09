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

ClassSpec StringContent::classSpec_ = {&Content::classSpec(),"StringContent",};
Reanimator<StringContent> StringContent::reanimator_;


StringContent::StringContent(const string& s): 
	str_(s) 
{ 
}

StringContent::StringContent(const char* s): 
	str_(s) 
{ 
}

StringContent::StringContent(Stream& s):
	Content(s)
{
	s >> str_;
}

void StringContent::encode(Stream& s) const
{
	Content::encode(s);
	s << str_;
}

StringContent::~StringContent()
{
}

void StringContent::print(ostream& s) const
{
	s << str_;
}

int StringContent::compare(const Content& other) const
{
	return -other.compareString(*this);
}

int StringContent::compareString(const StringContent& other) const
{
	return ::strcmp(str_.c_str(),other.str_.c_str());
}

void StringContent::value(string& s) const 
{ 
	s = str_; 
}

Content* StringContent::add(const Content& other) const
{
	return other.addString(*this);
}

Content* StringContent::addString(const StringContent& other) const
{
	return new StringContent(other.str_ + str_);
}

Content* StringContent::sub(const Content& other) const
{
	NOTIMP;
	return 0;
}

Content* StringContent::mul(const Content& other) const
{
	NOTIMP;
	return 0;
}

Content* StringContent::div(const Content& other) const
{
	NOTIMP;
	return 0;
}

