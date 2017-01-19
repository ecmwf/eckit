/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FileSystem.h
// MARS (Baudouin Raoult) - ECMWF Nov 01

#ifndef eckit_FileSystem_h
#define eckit_FileSystem_h

#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileSystem {
public:

// -- Exceptions
	// None

// -- Contructors

	FileSystem(const PathName&);

// -- Destructor

	~FileSystem(); 

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	void update();

	std::string  name()       const { return name_; }
	std::string  mountPoint() const;

	unsigned long long  total() const;
	unsigned long long  avail() const;
	long usage() const;
	bool offLine() const { return offLine_; }

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
	
	void print(std::ostream&) const; 	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

//	FileSystem(const FileSystem&);
//	FileSystem& operator=(const FileSystem&);

// -- Members
	// None

	PathName name_;
	PathName mount_;
	FileSystemSize fs_;
	bool offLine_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const FileSystem& p)
		{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
