/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/ClimateDate.h"
#include "eckit/compat/StrStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Climate date is a 30 day date

static Date makeDate(long year,long month,long day)
{

	ASSERT(month >= 1 && month <= 12);
	ASSERT(day   >= 1 && day   <= 30);

	long m = (month-1)*30 + day;
	ASSERT ( m >= 1 && m <= 12*30);

	return Date(year,m);
}

ClimateDate::ClimateDate(long year,long month,long day):
	date_(makeDate(year,month,day))
{
}

static Date makeDate(const string& s)
{
	Date date(s);
	long year = date.year();
	Date first(year,1,1);
	long day = (date - first + 1);

	ASSERT ( day >= 1 && day <= 12*30);

	return Date(year,day);
}

ClimateDate::ClimateDate(const string& s):
	date_(makeDate(s))
{
}

ClimateDate::operator string() const
{
	StrStream os;
	os << *this << StrStream::ends;
	return string(os);
}

void ClimateDate::print(ostream& s) const
{

	long year      = date_.year();
	long dayOfYear = date_ - Date(year,1,1);
#if 0
	long month     = dayOfYear / 30 + 1;
	long day       = dayOfYear % 30 + 1;

	char oldfill = s.fill();
	s << year << '-' << setw(2) << setfill('0') << month
		<< '-' << setw(2) << setfill('0') << day << setfill(oldfill);
#else

	char oldfill = s.fill();
	 s << year << '-' << setw(3) << setfill('0') << (dayOfYear + 1)
		<< setfill(oldfill);
#endif
}

void ClimateDate::dump(DumpLoad& a) const
{
	date_.dump(a);
}

void ClimateDate::load(DumpLoad& a)
{
	date_.load(a);
}

//-----------------------------------------------------------------------------

} // namespace eckit

