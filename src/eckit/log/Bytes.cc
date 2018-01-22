/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <limits>

#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"


namespace eckit {


//----------------------------------------------------------------------------------------------------------------------

Bytes::Bytes(double bytes):
	bytes_(bytes),
	rate_(false)
{
}

Bytes::Bytes(double bytes, Timer& timer) :
    bytes_(rate(bytes, timer.elapsed())),
    rate_(true) {
}

Bytes::Bytes(double bytes, double elapsed):
    bytes_(rate(bytes, elapsed)),
    rate_(true) {
}

double Bytes::rate(double num, double den) {

    if(num == 0.) return num;

    if(den == 0.) return num * std::numeric_limits<double>::infinity(); // must be after, num gives sign to inf

    return num / den;
}

//----------------------------------------------------------------------------------------------------------------------

static const char *b="kMGTPEZY"; // support until Yottabyte :-)
static const double yotta = 1024.*1024.*1024.*1024.*1024.*1024.*1024.;

static double rate(double x, char& c)
{
    if(x > yotta || std::isinf(x)) {
        c = 'Y';
        return 1.;
    }

    c = ' ';
    const char* p = b;
    c = ' ';
    while(x > 100)
    {
        x /= 1024;
        c = *p++;
    }
    if(x>=10)
        return long(x+0.5);
    else
        return long(x*10+0.5)/10.0;
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
    if(x != 1) s << 's';

    if(b.rate_) s << "/s";

	return s;
}

Bytes::operator std::string() const
{
    std::ostringstream s;
    s << *this;
    return s.str();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

