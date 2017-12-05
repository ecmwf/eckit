/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <sys/mman.h>

#include "eckit/eckit.h"
#include "eckit/memory/MMap.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void* MMap::mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    void* r = ::mmap(addr, length, prot, flags, fd, offset);
    if(r != MAP_FAILED) {
        instance().increment(length);
    }
    return r;
}

int MMap::munmap(void* addr, size_t length)
{
    int r = ::munmap(addr, length);
    if(r == 0) {
        instance().decrement(length);
    }
    return r;
}

MMap::MMap() :
    count_(0),
    maxCount_(0),
    length_(0),
    maxLength_(0)
{
}

MMap& MMap::instance() {
    static MMap mmap_;
    return mmap_;
}

void MMap::dump(std::ostream& os)
{
    MMap& mmap = instance();

    AutoLock<Mutex> lock(mmap.mutex_);

    os << "MMap"
       << " count " << mmap.count_
       << " max count " << mmap.maxCount_
       << " length " << mmap.length_
       << " max length " << mmap.maxLength_;
}

void MMap::increment(size_t len)
{
    AutoLock<Mutex> lock(mutex_);

    count_++;
    maxCount_ = std::max(count_, maxCount_);

    length_ += len;
    maxLength_ = std::max(length_, maxLength_);
}

void MMap::decrement(size_t len)
{
    AutoLock<Mutex> lock(mutex_);

    count_--;
    length_ -= len;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

