/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/DayOfYear.h"
#include "eclib/StrStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static long date2Value(const Date& date)
{
	Date jan01(date.year(),1,1);
	Date feb28(date.year(),2,28);
	Date nextJan01(date.year()+1,1,1);

	int numberOfDays = nextJan01 - jan01;

	long value = date - jan01;

	if(numberOfDays == 365 &&  value > feb28-jan01)
		value++;

	return value;
}

DayOfYear::DayOfYear(const Date& date):
	value_(date2Value(date))
{
}

DayOfYear::DayOfYear(const string& s):
	value_(date2Value(Date(s)))
{
}

DayOfYear::operator string() const
{
	StrStream os;
	os << *this << StrStream::ends;
	return string(os);
}

void DayOfYear::print(ostream& s) const
{
	Date d(2004,value_+1);         // Take a leap year so it can accept 29th feb

	char oldfill = s.fill();
	s << setw(2) << setfill('0') << d.month() << '-' << setw(2) << setfill('0') << d.day() 
	  << setfill(oldfill);
}

void DayOfYear::dump(DumpLoad& a) const
{
	NOTIMP;
}

void DayOfYear::load(DumpLoad& a)
{
	NOTIMP;
}

//-----------------------------------------------------------------------------

} // namespace eckit


