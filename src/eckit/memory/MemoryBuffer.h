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
/// @author Tiago Quintino
/// @date   July 1996

#ifndef eckit_memory_MemoryBlock_h
#define eckit_memory_MemoryBlock_h

#include <string>

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// A simple class to implement buffers

class MemoryBuffer : private NonCopyable {

public:  // methods

    MemoryBuffer(size_t size);
    MemoryBuffer(const std::string& s);
    MemoryBuffer(const char*, size_t size);

    ~MemoryBuffer();

    operator char*() { return static_cast<char*>(buffer_); }
    operator const char*() const { return static_cast<const char*>(buffer_); }

    operator void*() { return buffer_; }
    operator const void*() const { return buffer_; }

    void* data() { return buffer_; }
    const void* data() const { return buffer_; }

    size_t size() const { return size_; }

    void resize(size_t size);

    void swap(MemoryBuffer& rhs);

protected:  // methods

    void create();
    void destroy();

    void copy(const std::string& s);
    void copy(const char*, size_t size);

private:  // members

    void* buffer_;
    size_t size_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
