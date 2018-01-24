/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 11

#ifndef eckit_filesystem_marsfs_MarsFSClient_h
#define eckit_filesystem_marsfs_MarsFSClient_h

#include "eckit/net/Connector.h"
#include "eckit/io/Length.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FileSystemSize;
class MarsFSPath;

class MarsFSClient : private NonCopyable {
public:

// -- Contructors

    MarsFSClient(const MarsFSPath&);

// -- Destructor

	~MarsFSClient();

// -- Methods

    void mkdir(const std::string&,short);
    bool exists(const std::string&);
    std::string mountPoint(const std::string&);
    void fileSystemSize(const std::string& path, FileSystemSize& fs);
    void rename(const std::string&,const std::string&);
    void link(const std::string&,const std::string&);
    void unlink(const std::string&);
    void rmdir(const std::string&);
    void touch(const std::string&);
    void syncParentDirectory(const std::string&);
    std::string unique(const std::string&);
    Length size(const std::string&);
    bool sameAs(const std::string& path1, const std::string& path2);
    void match(const std::string& path,std::vector<std::string>& result,bool recurse);
    void children(const std::string& path,std::vector<std::string>& files, std::vector<std::string>& dirs);
    std::string baseName(const std::string& path, bool ext);
    std::string dirName(const std::string& path);
    std::string fullName(const std::string& path);


    time_t created(const std::string&);
    time_t lastAccess(const std::string&);
    time_t lastModified(const std::string&);


    std::vector<std::string> getFileSpaces();
    std::vector<std::string> getFileSystems(const std::string&);

protected:


// -- Members

	Connector& connector_;

private:

// -- Friends

	//friend std::ostream& operator<<(std::ostream& s,const MarsFSClient& p)
	//	{ p.print(s); return s; }
    friend class MarsFSClientCache;
};

//----------------------------------------------------------------------------------------------------------------------

class MarsFSClientRetry {
    bool old_;
public:
    MarsFSClientRetry(bool on);
    ~MarsFSClientRetry();
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
