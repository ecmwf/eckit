/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File CommandStream.h
// Baudouin Raoult - ECMWF May 96

#ifndef eclib_CommandStream_h
#define eclib_CommandStream_h


#include "eclib/Stream.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class CommandStream : public Stream {
public:

// -- Contructors
	
	CommandStream(const string& name,const char *mode);

// -- Destructor

	~CommandStream();

// -- Overridden methods
	
	// From Stream

	virtual long read(void*,long);
	virtual long write(const void*,long);

private:

// -- Members
	
	FILE* file_;

// -- Overridden methods

	// From Stream
	virtual string name() const;
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
