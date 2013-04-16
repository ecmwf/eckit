/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/VerifyingDate.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

VerifyingDate::VerifyingDate(const Date& d, const Time& t):
	DateTime(d,t)
{
	ASSERT( Second(time_) == 0 );
}

VerifyingDate::VerifyingDate(const string& s):
	DateTime(s)
{
	ASSERT( Second(time_) == 0 );
}


VerifyingDate::VerifyingDate(time_t thetime):
	DateTime(thetime)
{
	ASSERT( Second(time_) == 0 );
}

VerifyingDate::VerifyingDate(const DateTime& thetime):
	DateTime(thetime)
{
	ASSERT( Second(time_) == 0 );
}

VerifyingDate::operator string() const
{
	return string(date_);
}

void VerifyingDate::print(ostream& s) const
{
	s << date_;	
}

//-----------------------------------------------------------------------------

} // namespace eckit

