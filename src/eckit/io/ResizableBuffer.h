/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Jul 96

#ifndef eckit_io_ResizableBuffer_h
#define eckit_io_ResizableBuffer_h

#include "eckit/eckit.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A simple resizable buffer
/// Takes memory from mmap(/dev/zero),
/// thus easily recoverable from OS when released hopefully minimising memory fragmentation
/// Moreover we can trace how much we have allocated, @see Mmap wrapper

class ResizableBuffer : private eckit::NonCopyable {

public: // methods

	ResizableBuffer(size_t size);
	ResizableBuffer(const std::string& s);
	ResizableBuffer(const char*, size_t size);

	~ResizableBuffer();

	operator char*()                 { return (char*)buffer_; }
	operator const char*() const     { return (char*)buffer_; }

	operator void*()                 { return buffer_; }
	operator const void*() const     { return buffer_; }

	size_t size() const		 { return size_; }

    /// @post Invalidates contents of buffer
    void resize(size_t);

private: // methods

	void create();
	void destroy();

private: // members

    void*  buffer_;
	size_t size_;
	int    fd_;

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
