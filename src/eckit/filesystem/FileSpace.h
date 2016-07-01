/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   FileSpace.h
/// @date   Mar 1998
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_FileSpace_h
#define eckit_FileSpace_h

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileSpace : private NonCopyable {
public:

// -- Methods

	const PathName& sameFileSystem(const PathName&) const;
	const std::vector<PathName>& fileSystems() const { return fileSystems_; }

	bool owns(const PathName&) const;
	
	const PathName& selectFileSystem(const std::string&) const;
	const PathName& selectFileSystem()              const;

    const std::string& name() const { return name_; }

// -- Class methods

	static bool exists(const std::string&);
	static const FileSpace& lookUp(const std::string&);

private:

	FileSpace(const std::string&);
	~FileSpace(); 

// -- Methods

	void load() const;
	const PathName& find(const PathName&,bool&) const;
	

// -- Members
	
	std::string           name_;
	time_t			 last_;
	std::vector<PathName> fileSystems_;

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
