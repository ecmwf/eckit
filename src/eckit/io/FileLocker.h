/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FileLocker.h
// Baudouin Raoult - ECMWF Feb 02

#ifndef eckit_FileLocker_h
#define eckit_FileLocker_h

#include "eckit/machine.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileLocker {
public:

// -- Exceptions
	// None

// -- Contructors

	FileLocker(int fd);

// -- Destructor

	~FileLocker(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void lockShared(off64_t off = 0, off64_t len = 0);
	void lockExclusive(off64_t off = 0, off64_t len = 0);
	void unlock(off64_t off = 0, off64_t len = 0);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None


protected:

// -- Members
	// None

// -- Methods

	// void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	FileLocker(const FileLocker&);
	FileLocker& operator=(const FileLocker&);

// -- Members

	int fd_;

// -- Methods
	// None

	void lockRange(off64_t,off64_t,int cmd,int type);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const FileLocker& p)
	//	{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
