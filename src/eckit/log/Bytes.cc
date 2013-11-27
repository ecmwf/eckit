/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Bytes.h"
#include "eckit/compat/StrStream.h"
#include "eckit/utils/Timer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Bytes::Bytes(double bytes):
	bytes_(bytes),
	rate_(false)
{
}

Bytes::Bytes(double bytes,Timer& timer):
	bytes_(bytes/timer.elapsed()),
	rate_(true)
{
}

Bytes::Bytes(double bytes,double elapsed):
	bytes_(bytes/elapsed),
	rate_(true)
{
}

static const char *names[] = {"","K","M","G","T","P","E","Z","Y"};

std::ostream& operator<<(std::ostream& s, const Bytes& b)
{
	double x = b.bytes_;
	int    n = 0;

	if(x < 0)
	{
		s << '-';
		x = -x;
	}

    while(x>=1024.0 && (size_t) n < NUMBER(names) )
	{
		x /= 1024.0;
		n++;
	}

	s << x << ' ' << names[n] << "byte";

	if(x !=1 )  s << 's';
	if(b.rate_) s << " per seconds";

	return s;
}

Bytes::operator string() const
{
	StrStream s;
	s << *this << StrStream::ends;
	return string(s);
}

//-----------------------------------------------------------------------------

} // namespace eckit

