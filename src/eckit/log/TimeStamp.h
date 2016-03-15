/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TimeStamp.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_TimeStamp_h
#define eckit_TimeStamp_h

#include "eckit/eckit.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TimeStamp {
public:

// -- Contructors

	TimeStamp(const std::string& = defaultFormat_);
	TimeStamp(time_t,const std::string& = defaultFormat_);

// -- Operators

	operator std::string() const;

	friend std::ostream& operator<<(std::ostream&,const TimeStamp&);

private:

// There is no private copy constructor as this will confuse g++ 4.x.x

// -- Members

	time_t        time_;
	const std::string& format_;

// -- Class members

	static const char* defaultFormat_;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
