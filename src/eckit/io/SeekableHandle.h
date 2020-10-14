/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/SeekableHandle.h
// Baudouin Raoult - ECMWF May 2020

#ifndef eckit_filesystem_SeekableHandle_h
#define eckit_filesystem_SeekableHandle_h

#include <memory>

#include "eckit/io/HandleHolder.h"
#include "eckit/io/PeekHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class SeekableHandle : public DataHandle {
public:


    /// Contructor, taking ownership

	SeekableHandle(PeekHandle*);

    /// Contructor, not taking ownership

	SeekableHandle(PeekHandle&);

    /// Destructor

	virtual ~SeekableHandle();

// -- Operators

// -- Methods

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

	PeekHandle& handle_;
    size_t position_;
    bool owned_;

    virtual std::string title() const;

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
