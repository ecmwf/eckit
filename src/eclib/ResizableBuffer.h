/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResizableBuffer.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eclib_ResizableBuffer_h
#define eclib_ResizableBuffer_h

#include "eclib/machine.h"

// A simple class to implement buffers


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class ResizableBuffer {
public:

// -- Contructors

	ResizableBuffer(size_t size);
	ResizableBuffer(const string& s);
	ResizableBuffer(const char*,size_t size);

// -- Destructor

	~ResizableBuffer();

// -- Operators

	operator char*()                 { return (char*)buffer_; }
	operator const char*() const     { return (char*)buffer_; }

	operator void*()                 { return buffer_; }
	operator const void*() const     { return buffer_; }

// -- Methods

	size_t size() const		 { return size_; }
	void resize(size_t);

private:

// No copy allowed

	ResizableBuffer(const ResizableBuffer&);
	ResizableBuffer& operator=(const ResizableBuffer&);

// -- Methods

	void create();
	void destroy();

// -- Members

	void*  buffer_;
	size_t size_;
	int    fd_;

};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
