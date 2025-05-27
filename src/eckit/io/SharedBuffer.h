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
/// @date   July 2017

#ifndef eckit_io_SharedBuffer_h
#define eckit_io_SharedBuffer_h

#include <iosfwd>
#include <string>

#include "eckit/io/Buffer.h"
#include "eckit/memory/Counted.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A buffer that can be shared and is thread-safe
/// Note that this buffer, once shared should be for read-only shared-access (at the moment)
/// because access is not controlled by locking, only allocation and deallocation

class CountedBuffer : public eckit::Buffer, public eckit::Counted {
public:  // methods

    CountedBuffer(size_t size) : Buffer(size) {}
};

//----------------------------------------------------------------------------------------------------------------------

class SharedBuffer {
public:  // methods

    SharedBuffer(size_t size);

    SharedBuffer(CountedBuffer* b);

    ~SharedBuffer() { buffer_->detach(); }

    SharedBuffer(const SharedBuffer& s) : buffer_(s.buffer_) { buffer_->attach(); }

    SharedBuffer& operator=(const SharedBuffer& s) {
        if (&s != this) {
            buffer_->detach();
            buffer_ = s.buffer_;
            buffer_->attach();
        }
        return *this;
    }

    CountedBuffer* operator->() const { return buffer_; }

    void* data() { return buffer_->data(); }

    size_t size() const { return buffer_->size(); }

    // void resize(size_t size) { buffer_->resize(size); }

    /// Careful, use str() to convert the contents of a buffer to a string
    /// and don't rely on the contents to be null terminated
    /// This copies the data

    std::string str() const { return std::string(*buffer_, size()); }

    operator const Buffer&() const { return *buffer_; }
    operator Buffer&() { return *buffer_; }

private:  // methods

    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& s, const SharedBuffer& o) {
        o.print(s);
        return s;
    }

private:  // members

    CountedBuffer* buffer_;  ///< @invariant always a valid pointer
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
