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

/// Simple class to implement static size memory buffers

class Buffer : private NonCopyable {
public:  // methods
    Buffer() = default;
    explicit Buffer(size_t size);
    explicit Buffer(const std::string& s);

    Buffer(const char*, size_t size);

    Buffer(Buffer&& rhs) noexcept;
    Buffer& operator=(Buffer&& rhs) noexcept;

    ~Buffer();

    operator char*() { return static_cast<char*>(buffer_); }
    operator const char*() const { return static_cast<const char*>(buffer_); }

    operator void*() { return buffer_; }
    operator const void*() const { return buffer_; }

    void* data() { return buffer_; }
    const void* data() const { return buffer_; }

    size_t size() const { return size_; }

    void zero();

    /// @post Invalidates contents of buffer
    void resize(size_t, bool preserveData = false);

    /// Deallocate memory
    void clear();

    /// Copy data of given size (bytes) into buffer at given position
    /// @pre Buffer must have sufficient size
    void copy(const void*, size_t size, size_t pos = 0);

protected:  // methods
    void create();
    void destroy();

    void copy(const std::string& s);

private:  // members
    void* buffer_{nullptr};
    size_t size_{0};
};

}  // namespace eckit

#endif
