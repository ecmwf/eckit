/*
 * (C) Copyright 1996-2012 ECMWF.
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

#include "eclib/machine.h"
#include "eclib/Offset.h"
#include "eclib/Length.h"

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

	static FileManager& lookUp(const string&);

protected:

	FileManager(const string&);
	virtual ~FileManager();

	virtual void print(ostream&) const;

private:

	string name_;

	friend ostream& operator<<(ostream& s,const FileManager& p)
			{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

class FileManagerFactory {
	string name_;
	virtual FileManager* make(const string&) = 0 ;
public:
	static FileManager* build(const string&);
	FileManagerFactory(const string&);
	virtual ~FileManagerFactory();
};

template<class T>
class FileManagerBuilder : public FileManagerFactory {
	virtual FileManager* make(const string& name) { return new T(name); }
public:
	FileManagerBuilder(const string& name) : FileManagerFactory(name) {}
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
