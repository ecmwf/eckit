// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeMappedFile.h"

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <map>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/io/FileLock.h"
#include "eckit/log/Log.h"
#include "eckit/os/AutoUmask.h"


namespace eckit::geo::search {


static const TreeRegisterType<TreeMappedCacheFile> BUILDER_CACHE("mapped-cache-file");
static const TreeRegisterType<TreeMappedTempFile> BUILDER_TEMP("mapped-temporary-file");


const int TreeMappedFile::VERSION = 1;


namespace {


// one mutex per file, never released (the number of trees is bounded)
util::recursive_mutex& file_mutex(const std::string& path) {
    static util::recursive_mutex mutex;
    static std::map<std::string, std::unique_ptr<util::recursive_mutex>> mutexes;

    util::lock_guard<util::recursive_mutex> lock(mutex);

    auto& m = mutexes[path];
    if (!m) {
        m = std::make_unique<util::recursive_mutex>();
    }
    return *m;
}


// uid is part of a path, restrict to safe characters
bool safe_uid(const std::string& uid) {
    return !uid.empty() &&
           std::all_of(uid.begin(), uid.end(), [](unsigned char c) { return std::isalnum(c) || c == '-' || c == '_'; });
}


}  // namespace


TreeMappedFile::TreeMappedFile(const std::string& uid, size_t size, const std::vector<PathName>& roots) :
    TreeMapped(uid, size),
    path_(tree_path(uid, roots)),
    lock_path_(path_ + ".lock"),
    mutex_(file_mutex(lock_path_.asString())) {
    Log::debug() << "TreeMappedFile: '" << path_ << "'" << std::endl;
}


TreeMappedFile::~TreeMappedFile() {
    try {
        if (!tmp_.empty()) {
            close();
            PathName(tmp_).unlink(false);
        }
    }
    catch (const std::exception& e) {
        Log::warning() << "TreeMappedFile: failed to remove '" << tmp_ << "': " << e.what() << std::endl;
    }

    if (file_lock_) {
        file_lock_.reset();  // closing the lock file releases the process lock
        mutex_.unlock();
    }
}


PathName TreeMappedFile::tree_path(const std::string& uid, const std::vector<PathName>& roots) {
    if (!safe_uid(uid)) {
        throw exception::SearchError("TreeMappedFile: invalid uid '" + uid + "'", Here());
    }

    // shared cache, accessible to all
    AutoUmask umask(0);

    const auto relative = "search/" + std::to_string(VERSION) + "/" + uid + ".kdtree";

    for (const auto& root : roots) {
        if (!root.exists()) {
            try {
                root.mkdir(0777);
            }
            catch (const FailedSystemCall&) {
                continue;
            }
        }

        if (::access(root.localPath(), W_OK) != 0) {
            Log::debug() << "TreeMappedFile: root '" << root << "' isn't writable" << std::endl;
            continue;
        }

        return root / relative;
    }

    throw exception::SearchError("TreeMappedFile: no writable root for '" + relative + "'", Here());
}


void TreeMappedFile::build(std::vector<Value>& values) {
    create();
    TreeMapped::build(values);
}


void TreeMappedFile::insert(const Value& value) {
    if (!is_open()) {
        create();
    }
    TreeMapped::insert(value);
}


bool TreeMappedFile::ready() {
    if (is_open()) {
        return tmp_.empty();  // loaded, or being built
    }

    if (!path_.exists()) {
        return false;
    }

    if (!valid(path_)) {
        Log::warning() << "TreeMappedFile: invalid '" << path_ << "', rebuilding" << std::endl;
        return false;
    }

    open(path_, false);
    return true;
}


void TreeMappedFile::commit() {
    ASSERT(is_open() && !tmp_.empty());

    // unmap before publishing (atomically), then map the published file as any other reader would
    close();
    PathName::rename(tmp_, path_);
    tmp_.clear();

    open(path_, false);
}


void TreeMappedFile::lock() {
    mutex_.lock();

    try {
        // all access to the lock file within the process is serialised by mutex_, because closing any of its
        // descriptors would release the process (fcntl) lock
        file_lock_ = std::make_unique<FileLock>(lock_path_);
        file_lock_->lock();
    }
    catch (...) {
        file_lock_.reset();
        mutex_.unlock();
        throw;
    }
}


void TreeMappedFile::unlock() {
    ASSERT(file_lock_);

    file_lock_->unlock();
    file_lock_.reset();
    mutex_.unlock();
}


void TreeMappedFile::create() {
    ASSERT(!is_open());

    AutoUmask umask(0);
    tmp_ = PathName::unique(path_).asString();  // also creates the directory

    open(tmp_, true);
}


void TreeMappedFile::print(std::ostream& out) const {
    out << "TreeMappedFile[path=" << path_ << ",size=" << size() << "]";
}


TreeMappedCacheFile::TreeMappedCacheFile(const std::string& uid, size_t size) : TreeMappedFile(uid, size, roots()) {}


std::vector<PathName> TreeMappedCacheFile::roots() {
    return {PathName{LibEcKitGeo::cacheDir()}};
}


TreeMappedTempFile::TreeMappedTempFile(const std::string& uid, size_t size) : TreeMappedFile(uid, size, roots()) {}


std::vector<PathName> TreeMappedTempFile::roots() {
    return {PathName{"/tmp/eckit/geo"}};
}


}  // namespace eckit::geo::search
