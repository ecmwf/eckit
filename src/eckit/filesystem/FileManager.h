/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Length.h
// Baudouin Raoult - ECMWF Aug 98

#ifndef eckit_FileManager_h
#define eckit_FileManager_h

#include "eckit/eckit.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileName;
class DataHandle;

class FileManager {
public:

// - Methods

    virtual bool exists(const FileName&) = 0;
    
	virtual DataHandle*  newWriteHandle(const FileName&) = 0;
	virtual DataHandle*  newReadHandle(const FileName&) = 0;
	virtual DataHandle*  newReadHandle(const FileName&,const OffsetList&, const LengthList&) = 0;

	static FileManager& lookUp(const std::string&);

protected:

	FileManager(const std::string&);
	virtual ~FileManager();

	virtual void print(std::ostream&) const;

private:

	std::string name_;

	friend std::ostream& operator<<(std::ostream& s,const FileManager& p)
			{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

class FileManagerFactory {
	std::string name_;
	virtual FileManager* make(const std::string&) = 0 ;
public:
	static FileManager* build(const std::string&);
	FileManagerFactory(const std::string&);
	virtual ~FileManagerFactory();
};

template<class T>
class FileManagerBuilder : public FileManagerFactory {
	virtual FileManager* make(const std::string& name) { return new T(name); }
public:
	FileManagerBuilder(const std::string& name) : FileManagerFactory(name) {}
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
