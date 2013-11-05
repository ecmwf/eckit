/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef EmosFile_H
#define EmosFile_H

#include "eclib/DataHandle.h"
#include "eclib/NonCopyable.h"

// Forward declarations

namespace eckit { 

class EmosFile : private NonCopyable {

public: // methods

    /// Contructor

    EmosFile(const PathName&);

    /// Contructor, does not take ownership of eclib::DataHandle

    EmosFile( DataHandle& dh );

    /// Destructor

	~EmosFile();

public: // methods
	
	long read(eclib::Buffer&);

	// Don't fail if buffer is too small

	long readSome(eclib::Buffer&);

    eclib::Offset position();
	void rewind();

private: // members

    auto_ptr<DataHandle> handle_;

};

};

#endif
