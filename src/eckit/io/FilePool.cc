/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015

#include "eckit/io/FilePool.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/DataHandle.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static void closeDataHandle(PathName&, DataHandle*& handle) {
    if (handle) {
        handle->close();
        delete handle;
        handle = 0;
    }
}

static bool inUse(const std::map<PathName, DataHandle*>& store, const PathName& path) {
    return store.find(path) != store.end();
}

//----------------------------------------------------------------------------------------------------------------------

FilePool::FilePool(size_t size) :
    cache_(size, &closeDataHandle) {}

FilePool::~FilePool() {}

DataHandle* FilePool::checkout(const PathName& path) {
    AutoLock<MutexCond> lock(cond_);

    while (inUse(inUse_, path)) {
        cond_.wait();
    }

    DataHandle* dh;

    const bool inCache = cache_.exists(path);
    if (inCache && path.exists()) {
        dh = cache_.extract(path);
    }
    else {
        // FileHandle is cached, but file has been removed on disk
        if (inCache)
            cache_.remove(path);
        dh = path.fileHandle(false);  // append mode (no overwrite)
        dh->openForAppend(0);
    }

    ASSERT(dh);

    inUse_[path] = dh;

    return dh;
}

void FilePool::checkin(DataHandle* handle) {
    AutoLock<MutexCond> lock(cond_);

    typedef std::map<PathName, DataHandle*>::iterator iterator_type;
    for (iterator_type itr = inUse_.begin(); itr != inUse_.end(); ++itr) {
        if (itr->second == handle) {
            cache_.insert(itr->first, itr->second);
            inUse_.erase(itr);
            cond_.signal();
            return;
        }
    }
    throw eckit::SeriousBug("Should have found a DataHandle in pool use", Here());
}

bool FilePool::remove(const PathName& path) {
    AutoLock<MutexCond> lock(cond_);

    while (inUse(inUse_, path)) {
        cond_.wait();
    }

    return cache_.remove(path);
}

void FilePool::print(std::ostream& os) const {
    AutoLock<MutexCond> lock(const_cast<FilePool&>(*this).cond_);
    os << "FilePool("
       << "inUse=" << inUse_ << ", "
       << "cache=" << cache_ << ")";
}

size_t FilePool::size() const {
    AutoLock<MutexCond> lock(cond_);
    return cache_.size();
}

void FilePool::capacity(size_t size) {
    AutoLock<MutexCond> lock(cond_);
    cache_.capacity(size);
}

size_t FilePool::capacity() const {
    AutoLock<MutexCond> lock(cond_);
    return cache_.capacity();
}

size_t FilePool::usage() const {
    AutoLock<MutexCond> lock(cond_);
    return inUse_.size();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
