/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FileSpace.h
// Baudouin Raoult - ECMWF Mar 98

#ifndef eckit_FileSpace_h
#define eckit_FileSpace_h

#include "eclib/filesystem/PathName.h"
#include "eclib/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileSpace : private NonCopyable {
public:

// -- Methods

	const PathName& sameFileSystem(const PathName&) const;
	const vector<PathName>& fileSystems() const { return fileSystems_; }

	bool owns(const PathName&) const;
	
	const PathName& selectFileSystem(const string&) const;
	const PathName& selectFileSystem()              const;

    const string& name() const { return name_; }

// -- Class methods

	static bool exists(const string&);
	static const FileSpace& lookUp(const string&);

private:

	FileSpace(const string&);
	~FileSpace(); // Change to virtual if base class

// -- Methods

	void load() const;
	const PathName& find(const PathName&,bool&) const;
	

// -- Members
	
	string           name_;
	time_t			 last_;
	vector<PathName> fileSystems_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
