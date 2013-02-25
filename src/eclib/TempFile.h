/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TempFile.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef eclib_TempFile_h
#define eclib_TempFile_h

#include "eclib/PathName.h"


//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

class TempFile : public PathName {
public:

// -- Contructors

	TempFile();
	TempFile(const string&); /* the string is the name of a filespace */

// -- Destructor

	~TempFile();

private:

// -- Class member

	static PathName dir(const string&);
};


//-----------------------------------------------------------------------------

} // namespace eclib

#endif
