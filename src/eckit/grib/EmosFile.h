/*
 * (C) Copyright 1996-2013 ECMWF.
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

// Forward declarations

namespace eckit { class Buffer; }
namespace eckit { class PathName; }

class EmosFile : private eckit::NonCopyable {

public: // methods

    /// Contructor

    EmosFile(const eckit::PathName&);

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

private: // members

    auto_ptr<eckit::DataHandle> handle_;

};

#endif
