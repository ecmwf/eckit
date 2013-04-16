/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/LocalPathName.h
// B.Raoult - ECMWF May 96
#ifndef eckit_filesystem_LocalPathName_h
#define eckit_filesystem_LocalPathName_h

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Length;
struct FileSystemSize;
class DataHandle;
class BasePathName;

// The class LocalPathName represent a unix path name.

class LocalPathName {
public:

    friend void operator<<(Stream&,const LocalPathName&);
    friend void operator>>(Stream&,LocalPathName&);

    friend ostream& operator<<(ostream& s,const LocalPathName& p)
    {
        return s << p.path_;
    }

// Contructors

    LocalPathName(const char* p = "/") : path_(p) {
        tidy();
    }
    LocalPathName(const string& p)     : path_(p) {
        tidy();
    }
    LocalPathName(const LocalPathName& p)            : path_(p.path_) {         }

// Destructor
    // Implicite

// Assignment

    LocalPathName& operator=(const LocalPathName& p) {
        path_ = p.path_;
        return *this;
    }
    LocalPathName& operator=(const string& p)   {
        path_ = p      ;
        return tidy();
    }
    LocalPathName& operator=(const char* p)     {
        path_ = p      ;
        return tidy();
    }

// Convertors

    operator const string&() const {
        return path_;
    }
    const char* localPath() const  {
        return path_.c_str();
    }
    const char* c_str() const  {
        return path_.c_str();
    }

// Operators

    LocalPathName& operator+=(const string& s)  {
        path_ += s;
        return tidy();
    }
    LocalPathName& operator+=(const char* s)    {
        path_ += s;
        return tidy();
    }
    LocalPathName& operator+=(char s)           {
        path_ += s;
        return tidy();
    }

    bool operator<(const LocalPathName& other) const
    {
        return path_ < other.path_;
    }

    bool operator>(const LocalPathName& other) const
    {
        return path_ > other.path_;
    }

    bool operator<=(const LocalPathName& other) const
    {
        return path_ <= other.path_;
    }

    bool operator>=(const LocalPathName& other) const
    {
        return path_ >= other.path_;
    }

    bool operator!=(const LocalPathName& other) const
    {
        return path_ != other.path_;
    }

    bool operator==(const LocalPathName& other) const
    {
        return path_ == other.path_;
    }

// Methods

    // dirName() return the directory part of the path

    LocalPathName dirName() const;
    LocalPathName fullName() const;
    LocalPathName orphanName() const;
    BasePathName* checkClusterNode() const;

    string clusterName() const;

    // baseName() returns the name part of the path. If ext if false
    // the extension is stipped

    LocalPathName baseName(bool ext = true) const;

    bool exists() const;
    bool available() const;
    void touch() const;
    void reserve(const Length&) const;

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
    void copy(const LocalPathName&) const;

    bool sameAs(const LocalPathName&) const;
    LocalPathName mountPoint() const;
    LocalPathName realName() const;

    void children(vector<LocalPathName>&,vector<LocalPathName>&) const;
    const string& node() const;
    const string& path() const;

    void fileSystemSize(FileSystemSize&) const;
    DataHandle* fileHandle(bool overwrite) const;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    DataHandle* partHandle(const Offset&, const Length&) const;

// Class methods

    static LocalPathName unique(const LocalPathName&);
    static void match(const LocalPathName&,vector<LocalPathName>&,bool=false);
    static void link(const LocalPathName& from,const LocalPathName& to);
    static void rename(const LocalPathName& from,const LocalPathName& to);
    static void rename(const LocalPathName& from,const string& newBase);
    
    static LocalPathName cwd();

private:

// Members

    string path_;

// Methods

    LocalPathName& tidy();

// friend

    friend LocalPathName operator+(const LocalPathName& p,const string& s)
    {
        return LocalPathName(p.path_ + s);
    }

    friend LocalPathName operator+(const LocalPathName& p,const char* s)
    {
        return LocalPathName(p.path_ + s);
    }

    friend LocalPathName operator+(const LocalPathName& p,char s)
    {
        return LocalPathName(p.path_ + s);
    }


};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
