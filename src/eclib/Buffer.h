/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Buffer.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Buffer_h
#define eckit_Buffer_h

#include "eclib/machine.h"

#include "eclib/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// A simple class to implement buffers

class Buffer : private NonCopyable {
public:

// -- Contructors

	Buffer(size_t size);
	Buffer(const string& s);
	Buffer(const char*,size_t size);

// -- Destructor

	~Buffer();

// -- Operators

	operator char*()                 { return (char*)buffer_; }
	operator const char*() const     { return (char*)buffer_; }

	operator void*()                 { return buffer_; }
	operator const void*() const     { return buffer_; }

// -- Methods

	size_t size() const		 { return size_; }

private:

// -- Methods

	void create();
	void destroy();

// -- Members

    size_t size_;
	void*  buffer_;

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
