/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/NumberContent.h"
#include "eclib/Translator.h"

ClassSpec NumberContent::classSpec_ = {&Content::classSpec(),"NumberContent",};
Reanimator<NumberContent> NumberContent::reanimator_;

NumberContent::NumberContent(long long l): 
	num_(l)     
{
}

NumberContent::NumberContent(Stream& s):
	Content(s)
{
	s >> num_;
}

void NumberContent::encode(Stream& s) const
{
	Content::encode(s);
	s << num_;
}

NumberContent::~NumberContent()
{
}

void NumberContent::print(ostream& s) const
{
	s << num_;
}

int NumberContent::compare(const Content& other) const
{
	return -other.compareNumber(*this);
}

int NumberContent::compareNumber(const NumberContent& other) const
{
	// num_ can be a long long
	// return num_ - other.num_;
	long long dif = (num_ - other.num_);
	if(dif == 0)
		return dif;
	if(dif<0)
		return -1;

	return 1;
}

void NumberContent::value(long long& l) const 
{ 
	l = num_; 
}

void NumberContent::value(string& s) const 
{ 
	s = Translator<long long,string>()(num_); 
}

Content* NumberContent::add(const Content& other) const
{
	return other.addNumber(*this);
}

Content* NumberContent::addNumber(const NumberContent& other) const
{
	return new NumberContent(other.num_ + num_);
}

Content* NumberContent::sub(const Content& other) const
{
	return other.subNumber(*this);
}

Content* NumberContent::subNumber(const NumberContent& other) const
{
	return new NumberContent(other.num_ - num_);
}

Content* NumberContent::mul(const Content& other) const
{
	return other.mulNumber(*this);
}

Content* NumberContent::mulNumber(const NumberContent& other) const
{
	return new NumberContent(other.num_ * num_);
}

Content* NumberContent::div(const Content& other) const
{
	return other.divNumber(*this);
}

Content* NumberContent::divNumber(const NumberContent& other) const
{
	return new NumberContent(other.num_ / num_);
}

