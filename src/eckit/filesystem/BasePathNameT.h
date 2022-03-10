/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Simon Smart
/// @date   June 2011

#ifndef eckit_filesystem_BasePathNameT_h
#define eckit_filesystem_BasePathNameT_h

#include "eckit/filesystem/BasePathName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class BasePathNameT : public BasePathName {
public:
    // -- Contructors

    BasePathNameT(const T& path) : path_(path) {}

    BasePathNameT(const char* path, bool tildeIsUserHome = false) : path_(path, tildeIsUserHome) {}

    BasePathNameT(const std::string& path, bool tildeIsUserHome = false) : path_(path, tildeIsUserHome) {}

protected:
    // -- Methods

    void print(std::ostream&) const override;

private:
    // -- Members

    T path_;

    // -- Overridden methods

    virtual BasePathName* clone() const override;
    virtual const char* localPath() const override;
    virtual Length size() const override;
    virtual time_t lastAccess() const override;
    virtual time_t lastModified() const override;
    virtual time_t created() const override;

    virtual bool isDir() const override;
    virtual bool isLink() const override;

    virtual void rename(const BasePathName&) const override;
    virtual void link(const BasePathName&) const override;
    virtual bool sameAs(const BasePathName&) const override;
    virtual BasePathName* mountPoint() const override;
    virtual BasePathName* realName() const override;
    virtual bool exists() const override;
    virtual bool available() const override;
    virtual bool offsite() const override;
    virtual void mkdir(short) const override;
    virtual void chmod(const FileMode& mode) const override;
    virtual void unlink(bool verbose) const override;
    virtual void rmdir(bool verbose) const override;
    virtual void touch() const override;
    virtual void children(std::vector<BasePathName*>& files, std::vector<BasePathName*>& dirs) const override;
    virtual void match(std::vector<BasePathName*>&, bool) const override;
    virtual void reserve(const Length&) const override;

    virtual BasePathName* unique() const override;
    virtual const char* type() const override;
    virtual BasePathName* dirName() const override;
    virtual BasePathName* fullName() const override;
    virtual BasePathName* orphanName() const override;
    virtual BasePathName* checkClusterNode() const override;
    virtual BasePathName* baseName(bool) const override;
    virtual std::string extension() const override;
    virtual std::string hash(const std::string& method) const override;

    virtual std::string clusterName() const override;
    virtual void syncParentDirectory() const override;

    virtual std::string asString() const override;
    virtual void fileSystemSize(FileSystemSize&) const override;

    virtual DataHandle* fileHandle(bool overwrite) const override;
    virtual DataHandle* partHandle(const OffsetList&, const LengthList&) const override;
    virtual DataHandle* partHandle(const Offset&, const Length&) const override;

    virtual const std::string& node() const override;
    virtual const std::string& path() const override;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const BasePathNameT& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/filesystem/BasePathNameT.cc"

#endif
