/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FileStream.h
// Baudouin Raoult - ECMWF May 96

#ifndef FileStream_H
#define FileStream_H


#include "eclib/PathName.h"
#include "eclib/Stream.h"

class FileStream : public Stream {
public:

// -- Contructors
	
	FileStream(const PathName& name,const char *mode);

// -- Destructor

	~FileStream();

// -- Overridden methods
	
	// From Stream

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void rewind();

private:

// No copy allowed

	FileStream(const FileStream&);
	FileStream& operator=(const FileStream&);

protected:
// -- Members
	FILE* 		file_;
    bool        read_;
    PathName    name_;

// -- Overridden methods

	// From Stream
	virtual string name() const;
};

#endif
