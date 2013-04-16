/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Month.h"
#include "eclib/StrStream.h"
#include "eclib/Tokenizer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Month::Month(long year,long month):
	date_(year,month,1)
{
}

Month::Month(const Date& date):
	date_(date.year(),date.month(),1)
{
}

Month::Month(const string& s):
	date_(1997,10,10)
{
	Tokenizer parse("-");
	vector<string> result;

	parse(s,result);

	bool err = false;
	long value;

	switch(result.size())
	{
		case 1:
			switch(s.length())
			{
				case 6:
				case 8:
					value = atol(s.c_str());
					if((value%100) == 0) value++; // For dates as 970900
					date_ = Date(value);
					break;

				default:
					err = false;
					break;
			}
			break;

		case 2:
			if(result[0].length() != 2 && result[0].length() != 4) err = true;
			if(result[1].length() != 2) err = true;

			value = atol(result[0].c_str()) * 10000 + 
				    atol(result[1].c_str()) * 100   + 
					1;

			date_ = Date(value);

			break;

		case 3:

			if(result[0].length() != 2 && result[0].length() != 4) err = true;
			if(result[1].length() != 2) err = true;
			if(result[2].length() != 2) err = true;

			value = atol(result[0].c_str()) * 10000 + 
				    atol(result[1].c_str()) * 100   + 
					atol(result[2].c_str());

			if((value%100) == 0) value++; // For dates as 970900
			date_ = Date(value);

			break;

		default:
			err = false;
			break;
	}

	if(err) throw SeriousBug(string("Invalid month ") + s);
}

Month::operator string() const
{
	StrStream os;
	os << *this << StrStream::ends;
	return string(os);
}

void Month::print(ostream& s) const
{
	char oldfill = s.fill();
	s << year() << '-' << setw(2) << setfill('0') << month() 
	  << setfill(oldfill);
}

void Month::dump(DumpLoad& a) const
{
	date_.dump(a);
}

void Month::load(DumpLoad& a)
{
	date_.load(a);
}

//-----------------------------------------------------------------------------

} // namespace eckit

