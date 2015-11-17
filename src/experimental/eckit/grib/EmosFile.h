/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef EmosFile_H
#define EmosFile_H

#include "eckit/io/DataHandle.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/memory/ScopedPtr.h"

namespace eckit {

class Buffer;
class PathName;

namespace grib {

//------------------------------------------------------------------------------------------------------

class EmosFile : private eckit::NonCopyable {

public: // methods

    /// Contructor

    EmosFile(const eckit::PathName&, bool buffered);

    /// Contructor, does not take ownership of eckit::DataHandle

    EmosFile( eckit::DataHandle& dh );

    /// Destructor

	~EmosFile();

public: // methods
	
	long read(eckit::Buffer&);

	// Don't fail if buffer is too small

	long readSome(eckit::Buffer&);

    eckit::Offset position();
	void rewind();
    void seek(const eckit::Offset&);

private: // members

    eckit::ScopedPtr<eckit::DataHandle> handle_;

};

//------------------------------------------------------------------------------------------------------

}
}

#endif
