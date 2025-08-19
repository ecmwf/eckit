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

    BasePathName* clone() const override;
    const char* localPath() const override;
    Length size() const override;
    time_t lastAccess() const override;
    time_t lastModified() const override;
    time_t created() const override;

    bool isDir() const override;
    bool isLink() const override;

    void rename(const BasePathName&) const override;
    void link(const BasePathName&) const override;
    bool sameAs(const BasePathName&) const override;
    BasePathName* mountPoint() const override;
    BasePathName* realName() const override;
    bool exists() const override;
    bool available() const override;
    bool offsite() const override;
    void mkdir(short) const override;
    void chmod(const FileMode& mode) const override;
    void unlink(bool verbose) const override;
    void rmdir(bool verbose) const override;
    void touch() const override;
    void children(std::vector<BasePathName*>& files, std::vector<BasePathName*>& dirs) const override;
    void match(std::vector<BasePathName*>&, bool) const override;
    void reserve(const Length&) const override;

    BasePathName* unique() const override;
    const char* type() const override;
    BasePathName* dirName() const override;
    BasePathName* fullName() const override;
    BasePathName* orphanName() const override;
    BasePathName* checkClusterNode() const override;
    BasePathName* baseName(bool) const override;
    std::string extension() const override;
    std::string hash(const std::string& method) const override;

    std::string clusterName() const override;
    void syncParentDirectory() const override;

    std::string asString() const override;
    void fileSystemSize(FileSystemSize&) const override;

    DataHandle* fileHandle(bool overwrite) const override;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const override;
    DataHandle* partHandle(const Offset&, const Length&) const override;

    const std::string& node() const override;
    const std::string& path() const override;

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
