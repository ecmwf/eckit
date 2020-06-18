/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/PeekHandle.h
// Baudouin Raoult - ECMWF May 2020

#ifndef eckit_filesystem_PeekHandle_h
#define eckit_filesystem_PeekHandle_h

#include <deque>

#include "eckit/io/HandleHolder.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PeekHandle : public DataHandle, public HandleHolder {
public:


    /// Contructor, taking ownership

	PeekHandle(DataHandle*);

    /// Contructor, not taking ownership

	PeekHandle(DataHandle&);

    /// Destructor

	virtual ~PeekHandle();

// -- Operators

// -- Methods

    unsigned char peek(size_t);
    long peek(void* buffer, size_t size, size_t offset=0);

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();

    virtual long read(void*,long);
    virtual void close();

    virtual void rewind();
    virtual void print(std::ostream&) const;
    virtual void skip(const Length&);

    virtual Offset seek(const Offset&);
    virtual bool canSeek() const;

	virtual Length estimate();
	virtual Offset position();


private: // members

	std::deque<unsigned char> peek_;

    virtual std::string title() const;

};

//-----------------------------------------------------------------------------

class PeekWrapperHandle : public DataHandle {
public:

    PeekWrapperHandle(PeekHandle&);

    /// Destructor

    virtual ~PeekWrapperHandle();

    virtual long read(void*,long);

    virtual void rewind();
    virtual void print(std::ostream&) const;
    virtual void skip(const Length&);

    virtual Offset seek(const Offset&);
    virtual bool canSeek() const;

    // virtual Length estimate();
    virtual Offset position();

public:

    PeekHandle& handle_;
    size_t position_;

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
