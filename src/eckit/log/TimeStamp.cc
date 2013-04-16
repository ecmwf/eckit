/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/StrStream.h"
#include "eckit/log/TimeStamp.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

const char* TimeStamp::defaultFormat_ = "%Y-%m-%d %H:%M:%S";

TimeStamp::TimeStamp(const string& format):
	time_(time(0)),
	format_(format)
{
}

TimeStamp::TimeStamp(time_t t,const string& format):
	time_(t),
	format_(format)
{
}

ostream& operator<<(ostream& s,const TimeStamp&  x)
{
	char buf[80];
#ifdef EC_HAVE_GMTIME_R
	struct tm t;
	strftime(buf,sizeof(buf),x.format_.c_str(),gmtime_r(&x.time_,&t)); 
#else
	strftime(buf,sizeof(buf),x.format_.c_str(),gmtime(&x.time_)); 
#endif

	s << buf;
	
	return s;
}

TimeStamp::operator string() const
{
	StrStream s;
	s << *this << StrStream::ends;
	return string(s);
}

//-----------------------------------------------------------------------------

} // namespace eckit

