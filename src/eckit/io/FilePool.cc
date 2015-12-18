/*
 * (C) Copyright 1996-2015 ECMWF.
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
#include "eckit/thread/Mutex.h"
#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static void closeDataHandle(PathName& path, DataHandle*& handle)
{
    if(handle) {
        handle->close();
        delete handle;
        handle = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------

FilePool::FilePool(size_t size) :
    cache_(size, &closeDataHandle) {
}

FilePool::~FilePool() {
}

DataHandle *const FilePool::checkout(const PathName& path) {

    AutoLock<Mutex> lock(mutex_);

    ASSERT( ! inUse(path) );

    DataHandle* dh;

    if( cache_.exists(path) ) {
        dh = cache_.extract(path);
    }
    else {
        dh = path.fileHandle(false); // append mode (no overwrite)
        dh->openForAppend(0);
    }

    ASSERT(dh);

    addToInUse(path,dh);

    return dh;
}

void FilePool::checkin(DataHandle *const handle)
{
    AutoLock<Mutex> lock(mutex_);

    std::pair<PathName, DataHandle*> entry = removeFromInUse(handle);

    cache_.insert(entry.first, entry.second);
}

void FilePool::print(std::ostream& os) const
{
    AutoLock<Mutex> lock(const_cast<FilePool&>(*this).mutex_);
    os << "FilePool("
       << "inUse=" << inUse_ << ", "
       << "cache=" << cache_ << ")";
}

bool FilePool::inUse(const PathName& path) {
    return inUse_.find(path) != inUse_.end();
}

void FilePool::addToInUse(const PathName& path, DataHandle* dh) {
    inUse_[path] = dh;
}

std::pair<PathName, DataHandle*> FilePool::removeFromInUse(DataHandle* dh) {
    typedef std::map<PathName,DataHandle*>::iterator iterator_type;
    for(iterator_type itr = inUse_.begin(); itr != inUse_.end(); ++itr) {
        if( itr->second == dh ) {
            std::pair<PathName, DataHandle*> result = std::make_pair(itr->first, itr->second);
            inUse_.erase(itr);
            return result;
        }
    }
    throw eckit::SeriousBug("Should have found a DataHandle in pool use", Here());
}

size_t FilePool::size() const {
    return cache_.size();
}

void FilePool::capacity( size_t size ) {
    AutoLock<Mutex> lock(mutex_);
    cache_.capacity(size);
}

size_t FilePool::capacity() const {
    return cache_.capacity();
}

size_t FilePool::usage() const {
    return inUse_.size();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

