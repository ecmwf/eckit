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
/// @author Tiago Quintino
/// @date July 1996

#ifndef eckit_Buffer_h
#define eckit_Buffer_h

#include "eckit/eckit.h"

#include "eckit/memory/Counted.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// A simple class to implement buffers

class Buffer : public eckit::Counted {

public: // methods

    Buffer(size_t size);
    Buffer(const std::string& s);
    Buffer(const char*, size_t size);

    /// Constructor that does not take ownership of the data
    Buffer(void*, size_t size, bool dummy);

    ~Buffer();

    operator char*()                 { return static_cast<char*>(buffer_); }
    operator const char*() const     { return static_cast<const char*>(buffer_); }

    operator void*()                 { return buffer_; }
    operator const void*() const     { return buffer_; }

    void* data() { return buffer_; }

    size_t size() const		 { return size_; }

    void resize(size_t size);

    void swap(Buffer& rhs);

protected: // methods

    void create();
    void destroy();

    void copy(const std::string& s);
    void copy(const char*,size_t size);

private: // members

    void*  buffer_;
    size_t size_;

    bool   owned_;

};

//----------------------------------------------------------------------------------------------------------------------

/// A buffer that can be shared and is thread-safe
/// Note that this buffer, once shared should be for read-only shared-access (at the moment)
/// because access is not controlled by locking, only allocation and deallocation

class SharedBuffer {

public: // methods

    SharedBuffer(size_t size) : buffer_(new Buffer(size)) {
        buffer_->attach();
    }

    SharedBuffer(Buffer* b) {
        ASSERT(b);
        buffer_ = b;
        buffer_->attach();
    }

    ~SharedBuffer() {
        buffer_->detach();
    }

    SharedBuffer(const SharedBuffer& s) : buffer_(s.buffer_) {
        buffer_->attach();
    }

    SharedBuffer& operator=(const SharedBuffer& s) {
        if(buffer_) { buffer_->detach(); }
        buffer_ = s.buffer_;
        buffer_->attach();
        return *this;
    }

    operator void*()                 { return *buffer_; }
    operator const void*() const     { return *buffer_; }

    size_t size() const		 { return buffer_->size(); }

    /// Careful, use str() to convert the contents of a buffer to a string
    /// and don't rely on the contents to be null terminated

    std::string str() { return std::string(*buffer_, size()); }

private:

    Buffer* buffer_;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
