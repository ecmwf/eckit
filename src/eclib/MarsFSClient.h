/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MarsFSClient.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_MarsFSClient_h
#define eckit_MarsFSClient_h

#include "eclib/Connector.h"
#include "eclib/Length.h"
#include "eclib/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileSystemSize;
class MarsFSPath;

class MarsFSClient : private NonCopyable {
public:

// -- Contructors

    MarsFSClient(const MarsFSPath&);

// -- Destructor

	~MarsFSClient(); // Change to virtual if base class

// -- Methods
    
    void mkdir(const string&,short);
    bool exists(const string&);
    string mountPoint(const string&);
    void fileSystemSize(const string& path, FileSystemSize& fs);
    void rename(const string&,const string&);
    void link(const string&,const string&);
    void unlink(const string&);
    void rmdir(const string&);
    void touch(const string&);
    string unique(const string&);
    Length size(const string&);
    bool sameAs(const string& path1, const string& path2);
    void match(const string& path,vector<string>& result,bool recurse);
    void children(const string& path,vector<string>& dirs, vector<string>& files);
    string baseName(const string& path, bool ext);
    string dirName(const string& path);
    string fullName(const string& path);


    time_t created(const string&);
    time_t lastAccess(const string&);
    time_t lastModified(const string&);


    vector<string> getFileSpaces();
    vector<string> getFileSystems(const string&);

protected:


// -- Members

	Connector& connector_;

private:

// -- Friends

	//friend ostream& operator<<(ostream& s,const MarsFSClient& p)
	//	{ p.print(s); return s; }
    friend class MarsFSClientCache;
};

class MarsFSClientRetry {
    bool old_;
public:
    MarsFSClientRetry(bool on);
    ~MarsFSClientRetry();
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
