/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File serialisation/MemoryStream.h
// Manuel Fuentes - ECMWF Apr 16

#ifndef eckit_serialisation_MemoryStream_H
#define eckit_serialisation_MemoryStream_H

#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {
class Buffer;

//-----------------------------------------------------------------------------

class MemoryStream : public Stream {
public:



	MemoryStream(const Buffer&);
    MemoryStream(Buffer&);

	MemoryStream(const void* address,size_t size);
    MemoryStream(void* address,size_t size);

    /// Destructor

	~MemoryStream();

// -- Operators

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void rewind();
	virtual std::string name() const;


// -- Overridden methods


private: // members

	char*          address_;
    const size_t   size_;

    bool           read_;
    size_t         position_;


};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
