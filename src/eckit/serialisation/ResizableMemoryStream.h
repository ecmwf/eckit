/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   Feb 2019

#ifndef eckit_serialisation_ResizableMemoryStream_H
#define eckit_serialisation_ResizableMemoryStream_H

#include "eckit/serialisation/Stream.h"

namespace eckit {

class ResizableBuffer;

//----------------------------------------------------------------------------------------------------------------------

class ResizableMemoryStream : public Stream {
public:

    ResizableMemoryStream(ResizableBuffer&);

	~ResizableMemoryStream();

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void rewind();

	virtual std::string name() const;

    size_t position() const;

private: // members

    ResizableBuffer& buffer_;

    bool           read_;
    size_t         position_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit


#endif
