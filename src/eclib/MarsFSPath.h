/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MarsFSPath.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_MarsFSPath_h
#define eckit_MarsFSPath_h

#include "eclib/machine.h"

#include "eclib/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class BasePathName;

class MarsFSPath {
public:

// -- Contructors

	MarsFSPath(const string&);
	MarsFSPath(const string&, const string&);
	/* MarsFSPath(const PathName&); */
	MarsFSPath(const MarsFSPath&);
	MarsFSPath(Stream&);

	MarsFSPath& operator=(const MarsFSPath&);

// -- Destructor

	~MarsFSPath(); // Change to virtual if base class

// -- Operators

    bool operator==(const MarsFSPath& other) const
        { return node_ == other.node_ && path_ == other.path_; }

    bool operator!=(const MarsFSPath& other) const
        { return node_ != other.node_ || path_ != other.path_; }

// -- Methods

    const string& node() const { return node_; }
    const string& path() const { return path_; }
    //const string& name() const { return path_; }

    bool isLocal() const;
    bool isRemote() const { return !isLocal(); }

    // For PathName compatibility

    operator  string() const;
    const char* localPath() const;
    
    MarsFSPath baseName(bool = true) const;
    MarsFSPath dirName() const;
    MarsFSPath fullName() const;
    MarsFSPath orphanName() const;
    BasePathName* checkClusterNode() const;

    string clusterName() const;

    bool exists() const;
    bool available() const;
    void touch() const;
    void reserve(const Length&) const;

    Length size() const;
    time_t lastAccess()   const;
    time_t lastModified() const;
    time_t created()      const;

    bool isDir() const;
    
    void mkdir(short mode = 0777) const;
    void unlink() const;
    void rmdir() const;

    bool sameAs(const MarsFSPath&) const;
    MarsFSPath mountPoint() const;

    void children(vector<MarsFSPath>&,vector<MarsFSPath>&) const;

    static MarsFSPath unique(const MarsFSPath&);
    static void match(const MarsFSPath&,vector<MarsFSPath>&,bool=false);
    static void link(const MarsFSPath& from,const MarsFSPath& to);
    static void rename(const MarsFSPath& from,const MarsFSPath& to);

    void fileSystemSize(FileSystemSize&) const;


    DataHandle* fileHandle(bool overwrite) const;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    DataHandle* partHandle(const Offset&, const Length&) const;

protected:

// -- Methods
	
	void print(ostream&) const; // Change to virtual if base class	

private:

// -- Members

    string  node_;
    string  path_;

// -- Friends

	friend ostream& operator<<(ostream& s,const MarsFSPath& p)
		{ p.print(s); return s; }

	friend void operator<<(Stream&,const MarsFSPath&);
//	friend void operator>>(Stream&,MarsFSPath&);

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
