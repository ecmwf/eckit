/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_filesystem_MemoryHandle_h
#define eckit_filesystem_MemoryHandle_h

#include "eckit/io/DataHandle.h"

namespace eckit {
class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class MemoryHandle : public DataHandle {
public:

    MemoryHandle(const Buffer&);
    MemoryHandle(Buffer&);

    MemoryHandle(const void* address, size_t size);
    MemoryHandle(void* address, size_t size);


    MemoryHandle(size_t size = 1024 * 1024, bool grow = true);

    virtual ~MemoryHandle();

    /// Access the underlying buffer
    /// Size is provided by both the non-const overriden virtual function size()
    /// and the const version (available only on this class)
    const void* data() const;
    std::string str() const;
    Length size() const;

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*, long);
    virtual long write(const void*, long);
    virtual void close();
    virtual void flush();
    virtual void rewind();
    virtual void print(std::ostream&) const;
    virtual void skip(const Length&);

    virtual Offset seek(const Offset&);

    virtual Length size();
    virtual Length estimate();
    virtual Offset position();

    virtual DataHandle* clone() const;

private: // members

    char*          address_;
    size_t         size_;
    size_t         capacity_;

    bool           opened_;
    bool           readOnly_;
    bool           read_;
    bool           grow_;
    bool           owned_;

    Offset         position_;

    virtual std::string title() const;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit


#endif
