// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/MappedFile.h"

#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <ostream>

#include "eckit/container/KDMapped.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/cache/Version.h"
#include "eckit/io/FileLock.h"
#include "eckit/log/Log.h"
#include "eckit/os/AutoUmask.h"


namespace eckit::geo::search {


static const KDTreeBuilder<MappedFile> BUILDER_1("mmap");
static const KDTreeBuilder<MappedFile> BUILDER_2("mapped-memory");


namespace {


// one mutex per file, never released (the number of k-d trees is bounded)
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


MappedFile::MappedFile(const std::string& uid, size_t size) :
    KDTree(uid, size),
    path_(kdtree_path(uid, PathName{LibEcKitGeo::cacheDir()})),
    lock_path_(path_ + ".lock"),
    mutex_(file_mutex(lock_path_.asString())) {
    Log::debug() << "MappedFile: '" << path_ << "'" << std::endl;
}


MappedFile::~MappedFile() {
    try {
        if (!tmp_.empty()) {
            close();
            PathName(tmp_).unlink(false);
        }
    }
    catch (const std::exception& e) {
        Log::warning() << "MappedFile: failed to remove '" << tmp_ << "': " << e.what() << std::endl;
    }

    if (file_lock_) {
        file_lock_.reset();  // closing the lock file releases the process lock
        mutex_.unlock();
    }
}


void MappedFile::build(std::vector<Value>& values) {
    create();

    ASSERT(values.size() <= size());  // storage capacity

    // native value type, avoiding conversions while partitioning
    std::vector<Container::Value> v(values.begin(), values.end());
    mapped().build(v);
    count_ = v.size();
}


void MappedFile::insert(const Value& value) {
    if (!mapped_) {
        create();
    }

    ASSERT(count_ < size());  // storage capacity
    mapped().insert(value);
    ++count_;
}


Neighbour MappedFile::nearest_neighbour(const Point& p) {
    return to_neighbour(mapped().nearestNeighbour(p));
}


Neighbours MappedFile::k_nearest_neighbours(const Point& p, size_t k) {
    return to_neighbours(mapped().kNearestNeighbours(p, k));
}


Neighbours MappedFile::find_in_sphere(const Point& p, double radius) {
    return to_neighbours(mapped().findInSphere(p, radius));
}


bool MappedFile::ready() {
    if (mapped_) {
        return tmp_.empty();  // loaded, or being built
    }

    if (!path_.exists()) {
        return false;
    }

    if (!valid()) {
        Log::warning() << "MappedFile: invalid '" << path_ << "', rebuilding" << std::endl;
        return false;
    }

    open(path_, false);
    return true;
}


void MappedFile::commit() {
    ASSERT(mapped_ && !tmp_.empty());

    // unmap before publishing (atomically), then map the published file as any other reader would
    close();
    PathName::rename(tmp_, path_);
    tmp_.clear();

    open(path_, false);
}


void MappedFile::lock() {
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


void MappedFile::unlock() {
    ASSERT(file_lock_);

    file_lock_->unlock();
    file_lock_.reset();
    mutex_.unlock();
}


void MappedFile::stats_print(std::ostream& out, bool pretty) const {
    mapped().statsPrint(out, pretty);
}


void MappedFile::stats_reset() {
    mapped().statsReset();
}


size_t MappedFile::storage_size(size_t size) {
    constexpr size_t item = sizeof(Container::Node);

    // as KDMapped: header (no metadata) aligned to item size, then (unused) item 0 and items
    constexpr size_t base = ((sizeof(KDMappedHeader) + item - 1) / item) * item;
    return base + (size + 1) * item;
}


PathName MappedFile::kdtree_path(const std::string& uid, const PathName& root) {
    if (!safe_uid(uid)) {
        throw exception::SearchError("MappedFile: invalid uid '" + uid + "'", Here());
    }

    // shared cache, accessible to all
    AutoUmask umask(0);

    if (!root.exists()) {
        try {
            root.mkdir(0777);
        }
        catch (const FailedSystemCall& e) {
            throw exception::SearchError("MappedFile: cannot create '" + root.asString() + "': " + e.what(), Here());
        }
    }

    if (::access(root.localPath(), W_OK) != 0) {
        throw exception::SearchError("MappedFile: '" + root.asString() + "' isn't writable", Here());
    }

    return root / "search" / std::to_string(cache::version::SEARCH) / (uid + ".kdtree");
}


void MappedFile::close() {
    mapped_.reset();
    count_ = 0;
}


void MappedFile::open(const PathName& path, bool create) {
    ASSERT(!mapped_);

    // shared cache, accessible to all
    AutoUmask umask(0);

    mapped_ = std::make_unique<Container>(path, create ? size() : 0, 0);
    count_  = create ? 0 : size();
}


void MappedFile::create() {
    ASSERT(!mapped_);

    AutoUmask umask(0);
    tmp_ = PathName::unique(path_).asString();  // also creates the directory

    open(tmp_, true);
}


bool MappedFile::valid() const {
    std::ifstream in(path_.localPath(), std::ios::binary);

    KDMappedHeader header(0, 0, 0);
    if (!in.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        return false;
    }

    return header.headerSize_ == sizeof(KDMappedHeader) && header.itemSize_ == sizeof(Container::Node) &&
           header.itemCount_ == size() && header.metadataSize_ == 0 &&
           static_cast<long long>(path_.size()) == static_cast<long long>(storage_size(size()));
}


MappedFile::Container& MappedFile::mapped() {
    return const_cast<Container&>(static_cast<const MappedFile&>(*this).mapped());
}


const MappedFile::Container& MappedFile::mapped() const {
    if (!mapped_) {
        throw exception::SearchError("MappedFile: not mapped", Here());
    }
    return *mapped_;
}


void MappedFile::print(std::ostream& out) const {
    out << "MappedFile[path=" << path_ << ",size=" << size() << "]";
}


}  // namespace eckit::geo::search
