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

    /// Creates a buffer with 'size' many bytes.
    /// @param size in bytes of the buffer.
    explicit Buffer(size_t size = 0);

    /// Creates a buffer from the string s.
    /// NOTE: the buffer will contain the string as a zero terminated c-string.
    /// I.e. the resulting size of buffer is s.size() + 1
    /// @param s, initial content of buffer, including null-terminator
    explicit Buffer(const std::string& s);

    /// Creates buffer with initial content
    /// @param src to copy bytes from
    /// @param size of data
    Buffer(const void* src, size_t size);

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

    /// Returns size of the buffer in bytes
    /// Note: The actual allocation held by this buffer may be larger
    /// if the buffer has been resized with 'preserveData' set to a smaller size than before.
    /// @return buffer size in bytes
    size_t size() const { return size_; }

    /// Zero content of buffer
    void zero();

    /// Resizes the buffer to newSize.
    /// When 'preserveData' is set:
    /// - Buffer will still hold the data contained before resize() was called.
    /// - If 'newSize' is smaller than the current 'size' the data will be truncated.
    /// With unset 'preserveData' the contents of the buffer will be discarded.
    void resize(size_t newSize, bool preserveData = false);

    /// Copy data of given size (bytes) into buffer at given position
    /// @pre Buffer must have sufficient size
    void copy(const void*, size_t size, size_t pos = 0);

protected:  // methods

    void create();
    void destroy();

    /// Copies contents of s into the buffer including a null-terminator at the end.
    /// This copy operation truncates s if the buffer is not big enough.
    /// @param s, string to be copied including null-terminator.
    void copy(const std::string& s);

private:  // members

    char* buffer_{nullptr};
    size_t size_{0};
};

}  // namespace eckit

#endif
