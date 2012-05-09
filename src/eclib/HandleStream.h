/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HandleStream.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef HandleStream_H
#define HandleStream_H

#include "eclib/DataHandle.h"
#include "eclib/Stream.h"


// Adaptor: allow Stream operations on a DataHandle;

class HandleStream : public Stream {
public:

// -- Contructors

	HandleStream(DataHandle& h) : handle_(h) {}

// -- Destructor

	~HandleStream() {}

private:

// No copy allowed

	HandleStream(const HandleStream&);
	HandleStream& operator=(const HandleStream&);

// -- Members
	
	DataHandle& handle_;

// -- Overridden methods

	// From Stream

    virtual long write(const void* buf,long len)
		{ return handle_.write(buf,len); }

	virtual long read(void* buf,long len)
		{ return handle_.read(buf,len); }

	virtual string name() const
		{ return "HandleStream"; }

};

#endif
