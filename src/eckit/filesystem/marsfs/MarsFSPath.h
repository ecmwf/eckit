/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "eckit/eckit.h"

#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class BasePathName;

class MarsFSPath {
public:

// -- Contructors

	MarsFSPath(const std::string&);
	MarsFSPath(const std::string&, const std::string&);
	/* MarsFSPath(const PathName&); */
	MarsFSPath(const MarsFSPath&);
	MarsFSPath(Stream&);

	MarsFSPath& operator=(const MarsFSPath&);

// -- Destructor

	~MarsFSPath();

// -- Operators

    bool operator==(const MarsFSPath& other) const
        { return node_ == other.node_ && path_ == other.path_; }

    bool operator!=(const MarsFSPath& other) const
        { return node_ != other.node_ || path_ != other.path_; }

// -- Methods

    const std::string& node() const { return node_; }
    const std::string& path() const { return path_; }
    //const std::string& name() const { return path_; }

    bool isLocal() const;
    bool isRemote() const { return !isLocal(); }

    // For PathName compatibility

    operator  std::string() const;
    const char* localPath() const;

    MarsFSPath baseName(bool = true) const;
    MarsFSPath dirName() const;
    MarsFSPath fullName() const;
    MarsFSPath orphanName() const;
    BasePathName* checkClusterNode() const;

    std::string clusterName() const;
    std::string extension() const;

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

    void syncParentDirectory() const;

    bool sameAs(const MarsFSPath&) const;
    MarsFSPath mountPoint() const;
    MarsFSPath realName() const;

    void children(std::vector<MarsFSPath>&,std::vector<MarsFSPath>&) const;

    static MarsFSPath unique(const MarsFSPath&);
    static void match(const MarsFSPath&,std::vector<MarsFSPath>&,bool=false);
    static void link(const MarsFSPath& from,const MarsFSPath& to);
    static void rename(const MarsFSPath& from,const MarsFSPath& to);

    void fileSystemSize(FileSystemSize&) const;


    DataHandle* fileHandle(bool overwrite) const;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    DataHandle* partHandle(const Offset&, const Length&) const;

protected:

// -- Methods

	void print(std::ostream&) const;

private:

// -- Members

    std::string  node_;
    std::string  path_;

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const MarsFSPath& p)
		{ p.print(s); return s; }

	friend void operator<<(Stream&,const MarsFSPath&);
//	friend void operator>>(Stream&,MarsFSPath&);

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
