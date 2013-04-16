/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "sys/time.h"

#include "eckit/log/Seconds.h"
#include "eclib/Timer.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

timeval operator-(const timeval& a,const timeval& b)
{
	timeval diff;

    diff.tv_sec  = a.tv_sec  - b.tv_sec;
    diff.tv_usec = a.tv_usec - b.tv_usec;

    if (diff.tv_usec < 0)
    {
          diff.tv_sec--;
          diff.tv_usec += 1000000;
    }

	return diff;
}


Timer::Timer(const string& name,ostream& out):
    name_(name),
    out_(out),
    cpu_(clock())
{
    gettimeofday(&start_,0);
}

Timer::~Timer()
{
    double  s   = elapsed();
    clock_t cpu =  clock();
    out_ << name_ << ": " 
		 << Seconds(s) << " elapsed, " 
		 << Seconds(double(cpu-cpu_)/CLOCKS_PER_SEC) 
		 << " cpu" << endl;
}

double Timer::elapsed()
{
    timeval stop;
    gettimeofday(&stop,0);  
    timeval diff = stop - start_;
    return (double)diff.tv_sec + ((double)diff.tv_usec / 1000000.);
}

//-----------------------------------------------------------------------------

} // namespace eckit

