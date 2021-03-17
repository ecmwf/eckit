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

#ifndef eckit_io_Buffer_h
#define eckit_io_Buffer_h

#include <cstddef>
#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

/// Simple class to implement memory buffers

class Buffer : private NonCopyable {
public:  // methods
    explicit Buffer(size_t size = 0);
    explicit Buffer(const std::string& s);

    /// Allocate and copy memory of given length in bytes
    Buffer(const void*, size_t len);

    /// Move constructor. Note that rhs is not guaranteed to be valid!
    Buffer(Buffer&& rhs) noexcept;

    /// Move assignment. Note that rhs is not guaranteed to be valid!
    Buffer& operator=(Buffer&& rhs) noexcept;

    ~Buffer();

    operator char*() { return buffer_; }
    operator const char*() const { return buffer_; }

    operator void*() { return buffer_; }
    operator const void*() const { return buffer_; }

    void* data() { return buffer_; }
    const void* data() const { return buffer_; }

    /// @return allocated size
    size_t size() const { return size_; }

    /// Zero content of buffer
    void zero();

    /// @post Invalidates contents of buffer
    void resize(size_t, bool preserveData = false);

    /// Copy data of given size (bytes) into buffer at given position
    /// @pre Buffer must have sufficient size
    void copy(const void*, size_t size, size_t pos = 0);

protected:  // methods
    void create();
    void destroy();

    void copy(const std::string& s);

private:  // members
    char* buffer_{nullptr};
    size_t size_{0};
};

}  // namespace eckit

#endif
