/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/ETA.h"
#include "eckit/compat/StrStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ETA::ETA(double ETA):
	ETA_(ETA)
{
}

ETA::ETA(const timeval& time):
	ETA_(time.tv_sec + time.tv_usec / 1000000.0)
{
}

ostream& operator<<(ostream& s,const ETA&  sec)
{
	double t = sec.ETA_;
    long n  = t;

    long hour = n / (60 * 60); n %= (60 * 60);
    long minutes = n / 60 ; n %= 60 ;

    s << hour << ':' << setfill('0') << setw(2) << minutes << ':'  << setfill('0') << setw(2) << n;

	return s;
}

ETA::operator string() const
{
	StrStream s;
	s << *this << StrStream::ends;
	return string(s);
}

//-----------------------------------------------------------------------------

} // namespace eckit
