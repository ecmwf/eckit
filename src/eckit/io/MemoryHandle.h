/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/MemoryHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_MemoryHandle_h
#define eckit_filesystem_MemoryHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {
class Buffer;

//-----------------------------------------------------------------------------

class MemoryHandle : public DataHandle {
public:



    MemoryHandle(const Buffer&);
    MemoryHandle(Buffer&);

    MemoryHandle(const void* address, size_t size);
    MemoryHandle(void* address, size_t size);


    MemoryHandle(size_t size = 1024 * 1024, bool grow = true);

    /// Destructor

    virtual ~MemoryHandle();

// -- Operators

// -- Methods

    const void* data() const;
    std::string str() const;
    size_t size() const;


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

    virtual Length estimate();
    virtual Offset position();

    virtual DataHandle* clone() const;

    // From Streamable

#if 0
    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
#endif

// -- Class methods

#if 0
    static  const ClassSpec&  classSpec()        { return classSpec_;}
#endif

private: // methods


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

// -- Class members

#if 0
    static  ClassSpec                 classSpec_;
    static  Reanimator<MemoryHandle>  reanimator_;
#endif

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
