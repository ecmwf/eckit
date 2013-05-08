/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Timer.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_Timer_h
#define eckit_Timer_h

#include "sys/time.h"

#include "eckit/log/Log.h"
#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// This stack object prints the elapse time between the call to
// its contructor and the call to its destructor

class Timer : private NonCopyable {
public:

	Timer(const string& name, ostream& = Log::info());

	~Timer();

	double elapsed();

    string name() const { return name_; }

private: // members
	
	string         name_;
	struct timeval start_;
	clock_t        cpu_;
	ostream&       out_;

private: // methods
	
	ostream& put(ostream&,double);

};

timeval operator-(const timeval&,const timeval&);

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
