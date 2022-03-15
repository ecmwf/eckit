/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/filesystem/FileMode.h"

#include <algorithm>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class T>
void BasePathNameT<T>::print(std::ostream& s) const {
    s << path_;
}

template <class T>
BasePathName* BasePathNameT<T>::clone() const {
    return new BasePathNameT<T>(path_);
}

template <class T>
const char* BasePathNameT<T>::localPath() const {
    return path_.localPath();
}

template <class T>
Length BasePathNameT<T>::size() const {
    return path_.size();
}

template <class T>
time_t BasePathNameT<T>::lastAccess() const {
    return path_.lastAccess();
}

template <class T>
time_t BasePathNameT<T>::lastModified() const {
    return path_.lastModified();
}

template <class T>
time_t BasePathNameT<T>::created() const {
    return path_.created();
}

template <class T>
bool BasePathNameT<T>::isDir() const {
    return path_.isDir();
}

template <class T>
bool BasePathNameT<T>::isLink() const {
    return path_.isLink();
}

template <class T>
void BasePathNameT<T>::rename(const BasePathName& other) const {
    const BasePathNameT<T>& o = dynamic_cast<const BasePathNameT<T>&>(other);
    T::rename(this->path_, o.path_);
}

template <class T>
void BasePathNameT<T>::link(const BasePathName& other) const {
    const BasePathNameT<T>& o = dynamic_cast<const BasePathNameT<T>&>(other);
    T::link(this->path_, o.path_);
}

template <class T>
bool BasePathNameT<T>::sameAs(const BasePathName& other) const {
    const BasePathNameT<T>* o = dynamic_cast<const BasePathNameT<T>*>(&other);
    if (!o)
        return false;
    return path_.sameAs(o->path_);
}

template <class T>
BasePathName* BasePathNameT<T>::mountPoint() const {
    return new BasePathNameT<T>(path_.mountPoint());
}

template <class T>
BasePathName* BasePathNameT<T>::realName() const {
    return new BasePathNameT<T>(path_.realName());
}

template <class T>
bool BasePathNameT<T>::exists() const {
    return path_.exists();
}

template <class T>
bool BasePathNameT<T>::available() const {
    return path_.available();
}

template <class T>
bool BasePathNameT<T>::offsite() const {
    return path_.offsite();
}

template <class T>
std::string BasePathNameT<T>::clusterName() const {
    return path_.clusterName();
}

template <class T>
void BasePathNameT<T>::mkdir(short mode) const {
    path_.mkdir(mode);
}

template <class T>
void BasePathNameT<T>::chmod(const FileMode& mode) const {
    path_.chmod(mode.mode());
}

template <class T>
void BasePathNameT<T>::unlink(bool verbose) const {
    path_.unlink(verbose);
}

template <class T>
void BasePathNameT<T>::syncParentDirectory() const {
    path_.syncParentDirectory();
}

template <class T>
void BasePathNameT<T>::rmdir(bool verbose) const {
    path_.rmdir(verbose);
}

template <class T>
void BasePathNameT<T>::touch() const {
    path_.touch();
}

template <class T>
void BasePathNameT<T>::children(std::vector<BasePathName*>& files, std::vector<BasePathName*>& dirs) const {
    std::vector<T> f;
    std::vector<T> d;

    path_.children(f, d);

    for (typename std::vector<T>::iterator j = f.begin(); j != f.end(); ++j)
        files.push_back(new BasePathNameT<T>(*j));

    for (typename std::vector<T>::iterator j = d.begin(); j != d.end(); ++j)
        dirs.push_back(new BasePathNameT<T>(*j));
}

template <class T>
void BasePathNameT<T>::match(std::vector<BasePathName*>& result, bool rec) const {
    std::vector<T> r;
    T::match(path_, r, rec);

    for (typename std::vector<T>::iterator j = r.begin(); j != r.end(); ++j)
        result.push_back(new BasePathNameT<T>(*j));
}

template <class T>
void BasePathNameT<T>::reserve(const Length& length) const {
    path_.reserve(length);
}

template <class T>
BasePathName* BasePathNameT<T>::unique() const {
    return new BasePathNameT<T>(T::unique(path_));
}

template <typename T>
const char* BasePathNameT<T>::type() const {
    return T::type();
}

template <class T>
BasePathName* BasePathNameT<T>::dirName() const {
    return new BasePathNameT<T>(path_.dirName());
}

template <class T>
BasePathName* BasePathNameT<T>::orphanName() const {
    return new BasePathNameT<T>(path_.orphanName());
}

template <class T>
BasePathName* BasePathNameT<T>::checkClusterNode() const {
    return path_.checkClusterNode();
}

template <class T>
BasePathName* BasePathNameT<T>::fullName() const {
    return new BasePathNameT<T>(path_.fullName());
}

template <class T>
BasePathName* BasePathNameT<T>::baseName(bool ext) const {
    return new BasePathNameT<T>(path_.baseName(ext));
}

template <class T>
std::string BasePathNameT<T>::extension() const {
    return path_.extension();
}

template <class T>
std::string BasePathNameT<T>::hash(const std::string& method) const {
    return path_.hash(method);
}

template <class T>
std::string BasePathNameT<T>::asString() const {
    return std::string(path_);
}

template <class T>
void BasePathNameT<T>::fileSystemSize(FileSystemSize& fs) const {
    path_.fileSystemSize(fs);
}

template <class T>
DataHandle* BasePathNameT<T>::fileHandle(bool overwrite) const {
    return path_.fileHandle(overwrite);
}

template <class T>
DataHandle* BasePathNameT<T>::partHandle(const OffsetList& o, const LengthList& l) const {
    return path_.partHandle(o, l);
}

template <class T>
DataHandle* BasePathNameT<T>::partHandle(const Offset& o, const Length& l) const {
    return path_.partHandle(o, l);
}


template <class T>
const std::string& BasePathNameT<T>::node() const {
    return path_.node();
}

template <class T>
const std::string& BasePathNameT<T>::path() const {
    return path_.path();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
