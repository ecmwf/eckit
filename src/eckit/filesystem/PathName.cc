/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string.h>

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/BasePathName.h"
#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/PathNameFactory.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/Length.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/utils/Hash.h"

namespace eckit {

PathName::PathName(const char* p, bool tildeIsUserHome) {
    path_ = PathNameFactory::build(p, tildeIsUserHome);
}

PathName::PathName(const std::string& p, bool tildeIsUserHome) {
    path_ = PathNameFactory::build(p, tildeIsUserHome);
}

PathName::PathName(const std::string& type, const std::string& p, bool tildeIsUserHome) {
    path_ = PathNameFactory::build(type, p, tildeIsUserHome);
}

PathName::PathName(const PathName& other) :
    path_(other.path_->clone()) {}

PathName::PathName(const LocalPathName& other) :
    path_(new BasePathNameT<LocalPathName>(other)) {}

PathName::PathName(BasePathName* path) :
    path_(path) {
    ASSERT(path);
}

PathName& PathName::operator=(const PathName& other) {
    if (this != &other) {
        delete path_;
        path_ = other.path_->clone();
    }
    return *this;
}

PathName& PathName::operator=(const std::string& s) {
    // TODO: Review me
    *this = PathName(s);
    return *this;
}

PathName& PathName::operator=(const char* s) {
    // TODO: Review me
    *this = PathName(s);
    return *this;
}

PathName::~PathName() {
    delete path_;
}


void PathName::print(std::ostream& s) const {
    s << *path_;
}

const char* PathName::localPath() const {
    return path_->localPath();
}

PathName PathName::clusterName() const {
    return PathName(path_->clusterName());
}

Length PathName::size() const {
    return path_->size();
}

time_t PathName::lastAccess() const {
    return path_->lastAccess();
}

time_t PathName::lastModified() const {
    return path_->lastModified();
}

time_t PathName::created() const {
    return path_->created();
}

uid_t PathName::owner() const {
    struct stat s;
    SYSCALL(::stat(path_->localPath(), &s));
    return s.st_uid;
}

bool PathName::isDir() const {
    return path_->isDir();
}

bool PathName::isLink() const {
    return path_->isLink();
}

bool PathName::exists() const {
    return path_->exists();
}

bool PathName::available() const {
    return path_->available();
}

bool PathName::offsite() const {
    return path_->offsite();
}

void PathName::chmod(const FileMode& mode) const {
    path_->chmod(mode);
}

void PathName::unlink(bool verbose) const {
    path_->unlink(verbose);
}

void PathName::syncParentDirectory() const {
    path_->syncParentDirectory();
}

void PathName::rmdir(bool verbose) const {
    path_->rmdir(verbose);
}

void PathName::touch() const {
    path_->touch();
}

void PathName::mkdir(short mode) const {
    path_->mkdir(mode);
}

void PathName::rename(const PathName& from, const PathName& to) {
    from.path_->rename(*to.path_);
}

void PathName::link(const PathName& from, const PathName& to) {
    from.path_->link(*to.path_);
}

void PathName::hash(eckit::Hash& hash) const {
    ASSERT(not isDir());
    std::unique_ptr<DataHandle> dh(fileHandle());
    dh->openForRead();
    eckit::AutoClose closer(*dh);
    Length size = this->size();
    eckit::Buffer buffer(64 * 1024 * 1024);
    Length len = 0;
    while (len < size) {
        Length chunk = dh->read(buffer, buffer.size());
        len += chunk;
        hash.add(buffer, chunk);
    }
    ASSERT(len == size);
}

std::string PathName::hash(const std::string& method) const {
    return path_->hash(method);
}

void PathName::children(std::vector<PathName>& files, std::vector<PathName>& dirs) const {
    std::vector<BasePathName*> f;
    std::vector<BasePathName*> d;

    path_->children(f, d);

    for (std::vector<BasePathName*>::iterator j = f.begin(); j != f.end(); ++j)
        files.push_back(PathName(*j));

    for (std::vector<BasePathName*>::iterator j = d.begin(); j != d.end(); ++j) {
        dirs.push_back(PathName(*j));
    }
}

void PathName::childrenRecursive(std::vector<PathName>& files, std::vector<PathName>& dirs) const {
    std::vector<PathName> f;
    std::vector<PathName> d;

    children(f, d);

    for (std::vector<PathName>::iterator j = f.begin(); j != f.end(); ++j)
        files.push_back(*j);

    for (std::vector<PathName>::iterator j = d.begin(); j != d.end(); ++j) {
        dirs.push_back(*j);
        j->childrenRecursive(files, dirs);
    }
}

void PathName::match(const PathName& path, std::vector<PathName>& result, bool rec) {
    std::vector<BasePathName*> v;
    path.path_->match(v, rec);
    for (std::vector<BasePathName*>::iterator j = v.begin(); j != v.end(); ++j)
        result.push_back(PathName(*j));
}

bool PathName::operator<(const PathName& other) const {
    return this->asString() < other.asString();
}

bool PathName::operator>(const PathName& other) const {
    return this->asString() > other.asString();
}

bool PathName::operator!=(const PathName& other) const {
    return this->asString() != other.asString();
}

bool PathName::operator==(const PathName& other) const {
    return this->asString() == other.asString();
}

bool PathName::operator<=(const PathName& other) const {
    return this->asString() <= other.asString();
}

bool PathName::operator>=(const PathName& other) const {
    return this->asString() >= other.asString();
}

PathName PathName::unique(const PathName& path) {
    return PathName(path.path_->unique());
}

const char* PathName::type() const {
    return path_->type();
}

PathName PathName::dirName() const {
    return PathName(path_->dirName());
}

PathName PathName::orphanName() const {
    return PathName(path_->orphanName());
}

PathName PathName::checkClusterNode() const {
    return PathName(path_->checkClusterNode());
}

PathName PathName::baseName(bool ext) const {
    return PathName(path_->baseName(ext));
}

std::string PathName::extension() const {
    return path_->extension();
}

PathName PathName::fullName() const {
    return PathName(path_->fullName());
}

void PathName::reserve(const Length& length) const {
    path_->reserve(length);
}

void PathName::fileSystemSize(FileSystemSize& fs) const {
    path_->fileSystemSize(fs);
}

PathName PathName::mountPoint() const {
    return PathName(path_->mountPoint());
}

PathName PathName::realName() const {
    return PathName(path_->realName());
}

bool PathName::sameAs(const PathName& other) const {
    return path_->sameAs(*other.path_);
}

DataHandle* PathName::fileHandle(bool overwrite) const {
    return path_->fileHandle(overwrite);
}

DataHandle* PathName::partHandle(const OffsetList& o, const LengthList& l) const {
    return path_->partHandle(o, l);
}

DataHandle* PathName::partHandle(const Offset& o, const Length& l) const {
    return path_->partHandle(o, l);
}

std::string PathName::asString() const {
    return path_->asString();
}

PathName operator+(const PathName& p, const std::string& s) {
    // TODO: delegate that to "path_"
    return PathName(p.asString() + s);
}

PathName operator+(const PathName& p, const char* s) {
    // TODO: delegate that to "path_"
    return PathName(p.asString() + s);
}

PathName operator+(const PathName& p, char s) {
    // TODO: delegate that to "path_"
    return PathName(p.asString() + s);
}

PathName operator/(const PathName& p, const std::string& s) {
    // TODO: delegate that to "path_"
    return PathName(p.asString() + "/" + s);
}

PathName operator/(const PathName& p, const char* s) {
    // TODO: delegate that to "path_"
    return PathName(p.asString() + "/" + s);
}

PathName operator/(const PathName& p, char s) {
    // TODO: delegate that to "path_"
    return PathName(p.asString() + "/" + s);
}

void operator<<(Stream& s, const PathName& path) {
    // TODO: delegate that to "path_"
    s << path.asString();
}

void operator>>(Stream& s, PathName& path) {
    // TODO: delegate that to "path_"
    std::string p;
    s >> p;
    path = PathName(p);
}

// TODO: Read from ~etc/disk/...

static const char* NAMES[] = {"/locked/", "/transfer/", "/defrag/", "/temp/", "/obstmp/",
                              "/infrequent/", "/prearc/", "/cache/", nullptr};

std::string PathName::shorten(const std::string& s) {

    size_t i = 0;
    while (NAMES[i]) {
        if (s.find(NAMES[i]) != std::string::npos) {
            return std::string("...") + NAMES[i] + "...";
        }
        i++;
    }

    return s.substr(0, 10) + "...";
}

std::string PathName::metricsTag(const std::string& name) {
    PathName path(name);


    size_t i = 0;
    while (NAMES[i]) {
        int pos = path.path().find(NAMES[i]);
        if (pos != std::string::npos) {
            std::ostringstream oss;
            oss << path.node() << ":";
            oss << path.path().substr(0, pos + ::strlen(NAMES[i]) - 1);
            return oss.str();
        }
        i++;
    }

    return path.asString();
}


const std::string& PathName::node() const {
    return path_->node();
}

const std::string& PathName::path() const {
    return path_->path();
}

PathName& PathName::operator/=(const std::string& s) {
    // TODO: Review me
    *this = PathName(this->asString() + "/" + s);
    return *this;
}

PathName& PathName::operator/=(const char* s) {
    // TODO: Review me
    *this = PathName(this->asString() + "/" + s);
    return *this;
}

PathName& PathName::operator/=(char s) {
    // TODO: Review me
    *this = PathName(this->asString() + "/" + s);
    return *this;
}
PathName& PathName::operator+=(const std::string& s) {
    // TODO: Review me
    *this = PathName(this->asString() + s);
    return *this;
}

PathName& PathName::operator+=(const char* s) {
    // TODO: Review me
    *this = PathName(this->asString() + s);
    return *this;
}

PathName& PathName::operator+=(char s) {
    // TODO: Review me
    *this = PathName(this->asString() + s);
    return *this;
}

}  // namespace eckit
