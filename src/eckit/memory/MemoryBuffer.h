// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   July 1996

#ifndef eckit_memory_MemoryBlock_h
#define eckit_memory_MemoryBlock_h

#include <string>

#include "eckit/eckit.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// A simple class to implement buffers

class MemoryBuffer {

public:  // methods

    MemoryBuffer(size_t size);
    MemoryBuffer(const std::string& s);
    MemoryBuffer(const char*, size_t size);

    MemoryBuffer(const MemoryBuffer&)            = delete;
    MemoryBuffer& operator=(const MemoryBuffer&) = delete;
    MemoryBuffer(MemoryBuffer&&)                 = delete;
    MemoryBuffer& operator=(MemoryBuffer&&)      = delete;

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
