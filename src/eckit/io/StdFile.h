/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StdFile.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_StdFile_h
#define eckit_StdFile_h

#include <stdio.h>

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Simple wrapper around a stdio file

class PathName;

class StdFile : private NonCopyable {
public:

	StdFile(const PathName& name,const std::string& mode = "r");

	~StdFile();

	operator FILE*() { return file_; } // don't call fclose !!!

private: // members

	FILE *file_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
