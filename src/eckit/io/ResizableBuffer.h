/*
 * (C) Copyright 1996- ECMWF.
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

#include <cstring>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

/// A simple resizable buffer
/// Takes memory from mmap(/dev/zero),
/// thus easily recoverable from OS when released hopefully minimising memory fragmentation
/// Moreover we can trace how much we have allocated, @see Mmap wrapper

class ResizableBuffer : private eckit::NonCopyable {

public: // methods

	ResizableBuffer(size_t size);
	ResizableBuffer(const char*, size_t size);

	~ResizableBuffer();

	operator char*()                 { return buffer_; }
	operator const char*() const     { return buffer_; }

	operator void*()                 { return buffer_; }
	operator const void*() const     { return buffer_; }

    char* data() { return buffer_; }
    const char* data() const { return buffer_; }

	size_t size() const		 { return size_; }

    void zero();

    /// @post Invalidates contents of buffer
    void resize(size_t, bool preserveData=false);

private: // methods

	static char* allocate(size_t);
    static void deallocate(char*);


private: // members

    char*  buffer_;
	size_t size_;

};

} // namespace eckit

#endif
