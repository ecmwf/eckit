/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/PathName.h
// B.Raoult - ECMWF May 96

#ifndef eckit_filesystem_PathName_h
#define eckit_filesystem_PathName_h

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/filesystem/FileSystemSize.h"
#include "eclib/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Length;
class BasePathName;
class DataHandle;
class LocalPathName;
class MarsFSPath;

// The class PathName represent a unix path name.

class PathName {
public:

	friend void operator<<(Stream&,const PathName&);
	friend void operator>>(Stream&,PathName&);
	
	friend ostream& operator<<(ostream& s,const PathName& p)
        { p.print(s); return s; }

// Contructors

	PathName(const char* p = "/");
	PathName(const string& p);
	PathName(const PathName& p);
    PathName(const LocalPathName&);
    PathName(const MarsFSPath&);

// Destructor
    ~PathName();

// Assignment

	PathName& operator=(const PathName& p);
	PathName& operator=(const string& p);
	PathName& operator=(const char* p);

// Convertors

	string asString() const;
	operator string() const { return asString(); }
	/* const char* c_str() const      { return path_.c_str(); } */

    const char* localPath() const;

// Operators

    PathName& operator+=(const string& s);
	PathName& operator+=(const char* s);
	PathName& operator+=(char s);

    PathName& operator/=(const string& s);
	PathName& operator/=(const char* s);
	PathName& operator/=(char s);

	bool operator<(const PathName& other) const;
	bool operator>(const PathName& other) const;
	bool operator<=(const PathName& other) const;
	bool operator>=(const PathName& other) const;
	bool operator!=(const PathName& other) const;
	bool operator==(const PathName& other) const;

// Methods

	/// @returns the directory part of the path
	PathName dirName() const;
    
	PathName fullName() const;
	PathName clusterName() const;

	/// @returns the name part of the path
    /// @param ext if false the extension is stripped
	PathName baseName(bool ext = true) const;

	bool exists() const;
	void touch() const;
	void reserve(const Length&) const;
	bool available() const;

	Length size() const;
	time_t lastAccess()   const;
	time_t lastModified() const;
	time_t created()      const;

	bool isDir() const;

	void empty() const;
	void truncate(Length) const;

	void mkdir(short mode = 0777) const;
	void chmod(short mode) const;
	void unlink() const;
	void rmdir() const;

	void backup() const;
	void copy(const PathName&) const;

	bool sameAs(const PathName&) const;
	PathName mountPoint() const;
	PathName realName() const;

	PathName orphanName() const;
	PathName checkClusterNode() const;

    const string& node() const;
    const string& path() const;

	void children(vector<PathName>&,vector<PathName>&) const;

	void fileSystemSize(FileSystemSize&) const;

    DataHandle* fileHandle(bool overwrite = false) const;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    DataHandle* partHandle(const Offset&, const Length&) const;

// Class methods

	static PathName unique(const PathName&);
	static void match(const PathName&,vector<PathName>&,bool=false);
	static void link(const PathName& from,const PathName& to);
	static void rename(const PathName& from,const PathName& to);
	static void rename(const PathName& from,const string& newBase);

    static string shorten(const string&);

private:

	PathName(BasePathName*);

// Members


    BasePathName* path_;

// Methods

	PathName& tidy();
    void print(ostream&) const;

// friend

	friend PathName operator+(const PathName& p,const string& s);
	friend PathName operator+(const PathName& p,const char* s);
	friend PathName operator+(const PathName& p,char s);

    friend PathName operator/(const PathName& p,const string& s);
    friend PathName operator/(const PathName& p,const char* s);
	friend PathName operator/(const PathName& p,char s);

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
