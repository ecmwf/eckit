/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef GribReader_H
#define GribReader_H

#include "eckit/eckit.h"
#include "eckit/io/StdFile.h"
#include "eckit/memory/NonCopyable.h"

// Forward declarations

namespace eckit { class Buffer; }
namespace eckit { class PathName; }

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

class GribReader : private eckit::NonCopyable {
public:

// -- Exceptions
	// None

// -- Contructors

	GribReader(const eckit::PathName&);

// -- Destructor

	~GribReader();

// -- Convertors
	// None

// -- Operators

// -- Methods
	

	// Don't fail if buffer is too small

	long readSome(eckit::Buffer&);

    void seek(const eckit::Offset&);

    eckit::Offset position();
	void rewind();

private:

    eckit::StdFile file_;
    unsigned long read_;
    unsigned long skip_;
    
};

//------------------------------------------------------------------------------------------------------

}
}

#endif
