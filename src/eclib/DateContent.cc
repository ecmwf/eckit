/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/DateContent.h"
#include "eclib/NumberContent.h"

ClassSpec DateContent::classSpec_ = {&Content::classSpec(),"DateContent",};
Reanimator<DateContent> DateContent::reanimator_;

DateContent::DateContent(const Date& d): 
	date_(d)
{
}

DateContent::DateContent(Stream& s):
	Content(s)
{
	string dd;
	s >> dd;
	date_ = Date(dd);
}

void DateContent::encode(Stream& s) const
{
	Content::encode(s);
	string dd = date_;
	s << dd;
}

DateContent::~DateContent()
{
}

void DateContent::print(ostream& s) const
{
	s << date_;
}

int DateContent::compare(const Content& other) const
{
	return -other.compareDate(*this);
}

int DateContent::compareDate(const DateContent& other) const
{
	if(date_ < other.date_)
		return -1;
	else if(date_ == other.date_)
		return 1;

	return 0;
}

void DateContent::value(Date& d) const 
{ 
	d = date_; 
}

Content* DateContent::add(const Content& other) const
{
	return other.addDate(*this);
}

Content* DateContent::addDate(const DateContent& other) const
{
	return new DateContent(date_ + other.date_);
}

Content* DateContent::sub(const Content& other) const
{
	return other.subDate(*this);
}

Content* DateContent::subDate(const DateContent& other) const
{
	return new NumberContent(date_ - other.date_);
}

Content* DateContent::mul(const Content& other) const
{
	NOTIMP;
	return 0;
}

Content* DateContent::div(const Content& other) const
{
	NOTIMP;
	return 0;
}

