/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StdPipe.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_StdPipe_h
#define eckit_StdPipe_h

#include <stdio.h>
#include <string>


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


// Simple wrapper around a stdio file

class StdPipe {
public:

// -- Contructors

	StdPipe(const std::string& name,const std::string& mode = "r");

// -- Destructor

	~StdPipe();

// -- Convertors

	operator FILE*() { return file_; } // don't call fclose !!!

private:

// No copy allowed

	StdPipe(const StdPipe&);
	StdPipe& operator=(const StdPipe&);

// -- Members

	FILE *file_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
