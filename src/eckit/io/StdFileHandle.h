/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Stdio/FileHandle.h
// Piotr Kuchta - ECMWF April 09

#ifndef eckit_filesystem_StdFileHandle_h
#define eckit_filesystem_StdFileHandle_h


#include "eckit/io/DataHandle.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StdFileHandle : public DataHandle {
public:

// -- Contructors

	StdFileHandle(FILE *);

// -- Destructor

	~StdFileHandle();

// -- Overridden methods

	// From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

	virtual long   read(void*,long);
	virtual long   write(const void*,long);
	virtual void   close();
	virtual void   print(ostream&) const;

	// From Streamable

	virtual void encode(Stream&) const;

// -- Class methods


private:

// -- Members

	FILE* f_;

// -- Class members

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
