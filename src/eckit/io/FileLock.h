/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Oct 2016

#ifndef eckit_FileLock_h
#define eckit_FileLock_h

#include "eckit/io/FileLocker.h"

namespace eckit {

class PathName;

//----------------------------------------------------------------------------------------------------------------------

class FileLock {
public:

// -- Exceptions
	// None

// -- Contructors

	// Will create the lock file if needed
	FileLock(const PathName& lockFile);

// -- Destructor

	~FileLock();

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void lock();
	void unlock();

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

	// void print(std::ostream&) const;

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	FileLock(const FileLock&);
	FileLock& operator=(const FileLock&);

// -- Members

	int fd_;
	FileLocker locker_;

// -- Methods
	// None


// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend std::ostream& operator<<(std::ostream& s,const FileLock& p)
	//	{ p.print(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
