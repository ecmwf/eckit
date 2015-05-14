/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/BasePathNameT.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_filesystem_BasePathNameT_h
#define eckit_filesystem_BasePathNameT_h

#include "eckit/filesystem/BasePathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class T>
class BasePathNameT : public BasePathName {
public:

// -- Contructors

	BasePathNameT(const T& path):
		path_(path) {}

	BasePathNameT(const char* path):
		path_(path) {}

	BasePathNameT(const std::string& path):
		path_(path) {}

protected:

// -- Methods

	void print(std::ostream&) const;

private:

// -- Members

	T path_;

// -- Overridden methods

    virtual BasePathName* clone() const;
    virtual const char* localPath() const;
    virtual Length size() const;
    virtual time_t lastAccess() const;
    virtual time_t lastModified() const;
    virtual time_t created() const;

    virtual bool isDir() const;

    virtual void rename(const BasePathName&) const;
    virtual void link(const BasePathName&) const;
    virtual bool sameAs(const BasePathName&) const;
    virtual BasePathName* mountPoint() const;
    virtual BasePathName* realName() const;
    virtual bool exists() const;
    virtual bool available() const;
    virtual void mkdir(short) const;
    virtual void unlink() const;
    virtual void rmdir() const;
    virtual void touch() const;
    virtual void children(std::vector<BasePathName*>&,std::vector<BasePathName*>&) const;
    virtual void match(std::vector<BasePathName*>&,bool) const;
    virtual void reserve(const Length&) const;

    virtual BasePathName* unique() const;
    virtual BasePathName* dirName() const;
    virtual BasePathName* fullName() const;
    virtual BasePathName* orphanName() const;
    virtual BasePathName* checkClusterNode() const;
    virtual BasePathName* baseName(bool) const;

    virtual std::string clusterName() const;
    virtual void syncParentDirectory() const;

    virtual std::string asString() const;
    virtual void fileSystemSize(FileSystemSize&) const;

    virtual DataHandle* fileHandle(bool overwrite) const;
    virtual DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    virtual DataHandle* partHandle(const Offset&, const Length&) const;

    virtual const std::string& node() const;
    virtual const std::string& path() const;

// -- Friends

    friend std::ostream& operator<<(std::ostream& s,const BasePathNameT& p)	{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit

#include "eckit/filesystem/BasePathNameT.cc"

#endif
