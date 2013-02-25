/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Timer.h
// Baudouin Raoult - ECMWF May 96

#ifndef eclib_Timer_h
#define eclib_Timer_h

#include "eclib/Log.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

// This stack object prints the elapse time between the call to
// its contructor and the call to its destructor

class Timer {
public:

// -- Contructors

	Timer(const string& name, ostream& = Log::info());

// -- Destructor

	~Timer();

// -- Methods

	double elapsed();

private:

// No copy allowed

	Timer(const Timer&);
	Timer& operator=(const Timer&);

// -- Members
	
	string         name_;
	struct timeval start_;
	clock_t        cpu_;
	ostream&       out_;

// -- Methods
	
	ostream& put(ostream&,double);

};

timeval operator-(const timeval&,const timeval&);


//-----------------------------------------------------------------------------

} // namespace eclib


#endif
