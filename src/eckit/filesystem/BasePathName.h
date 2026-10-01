// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date Jun 2011

#ifndef eckit_filesystem_BasePathName_h
#define eckit_filesystem_BasePathName_h

#include "eckit/eckit.h"

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"

namespace eckit {


class BasePathName {
public:

    BasePathName() {}

    BasePathName(const BasePathName&)            = delete;
    BasePathName& operator=(const BasePathName&) = delete;
    BasePathName(BasePathName&&)                 = delete;
    BasePathName& operator=(BasePathName&&)      = delete;

    virtual ~BasePathName();

    virtual BasePathName* clone() const   = 0;
    virtual const char* localPath() const = 0;
    virtual Length size() const           = 0;
    virtual time_t lastAccess() const     = 0;
    virtual time_t lastModified() const   = 0;
    virtual time_t created() const        = 0;

    virtual bool isDir() const  = 0;
    virtual bool isLink() const = 0;

    virtual void rename(const BasePathName&) const = 0;
    virtual void link(const BasePathName&) const   = 0;
    virtual bool sameAs(const BasePathName&) const = 0;
    virtual BasePathName* mountPoint() const       = 0;
    virtual BasePathName* realName() const         = 0;

    virtual bool exists() const                                                                      = 0;
    virtual bool available() const                                                                   = 0;
    virtual bool offsite() const                                                                     = 0;
    virtual void mkdir(short) const                                                                  = 0;
    virtual void chmod(const FileMode& mode) const                                                   = 0;
    virtual void unlink(bool verbose) const                                                          = 0;
    virtual void rmdir(bool verbose) const                                                           = 0;
    virtual void touch() const                                                                       = 0;
    virtual void children(std::vector<BasePathName*>& files, std::vector<BasePathName*>& dirs) const = 0;
    virtual void match(std::vector<BasePathName*>&, bool) const                                      = 0;
    virtual void reserve(const Length&) const                                                        = 0;

    virtual BasePathName* unique() const                      = 0;
    virtual const char* type() const                          = 0;
    virtual BasePathName* dirName() const                     = 0;
    virtual BasePathName* fullName() const                    = 0;
    virtual BasePathName* orphanName() const                  = 0;
    virtual BasePathName* checkClusterNode() const            = 0;
    virtual BasePathName* baseName(bool) const                = 0;
    virtual std::string extension() const                     = 0;
    virtual std::string hash(const std::string& method) const = 0;

    virtual std::string clusterName() const = 0;

    virtual void syncParentDirectory() const = 0;

    virtual std::string asString() const                                       = 0;
    virtual void fileSystemSize(FileSystemSize&) const                         = 0;
    virtual std::string fileSystemType() const                                 = 0;
    virtual DataHandle* fileHandle(bool overwrite) const                       = 0;
    virtual DataHandle* partHandle(const OffsetList&, const LengthList&) const = 0;
    virtual DataHandle* partHandle(const Offset&, const Length&) const         = 0;

    virtual const std::string& node() const = 0;
    virtual const std::string& path() const = 0;

protected:

    virtual void print(std::ostream&) const = 0;

private:

    friend std::ostream& operator<<(std::ostream& s, const BasePathName& p) {
        p.print(s);
        return s;
    }
};

}  // namespace eckit

#endif
