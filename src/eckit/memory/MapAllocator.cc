/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/mman.h>
#include <unistd.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/MapAllocator.h"

#include "eckit/memory/MMap.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static size_t whole_page(size_t size) {
    static size_t page = sysconf(_SC_PAGE_SIZE);
    return ((size + page - 1) / page) * page;
}

MapAllocatorTooSmall::MapAllocatorTooSmall(size_t, size_t) :
    Exception("MapAllocator too small") {}

MapAllocator::MapAllocator(size_t length) :
    fd_(-1), length_(whole_page(length)), count_(0), more_(0) {
    addr_ = MMap::mmap(0, length_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, fd_, 0);

    if (addr_ == MAP_FAILED)
        throw FailedSystemCall("mmap", Here());

    next_ = (char*)addr_;
    left_ = length_;
    //	Log::warning() << "MapAllocator created " << length_ << std::endl;
}

MapAllocator::~MapAllocator() {
    //	Log::warning() << "MapAllocator deleted " << length_ << std::endl;
    munmap(addr_, length_);
    if (fd_ >= 0)
        close(fd_);
    delete more_;
}

union Align {
    char char_;
    double double_;
    long long longlong_;
    void* voidstar_;
    float float_;
};

const int WORD = sizeof(Align);

void* MapAllocator::allocate(size_t size) {
    // return ::operator new(size);

    size = ((size + WORD - 1) / WORD) * WORD;

    if (size > left_) {
        if (!more_) {
            //			Log::warning() << "MapAllocator too small: length=" << length_ << " left=" << left_ << "
            // request="
            //<< size << std::endl;
            more_ = new MapAllocator(std::max(length_, size));
            ASSERT(more_);
        }
        return more_->allocate(size);
    }

    char* addr = next_;
    next_ += size;
    left_ -= size;
    count_++;

    return addr;
}

void MapAllocator::deallocate(void* addr) {
    //::operator delete(addr); return;

    char* p = (char*)addr;
    char* q = (char*)addr_;

    if (p >= q && p < q + length_) {
        count_--;
        if (count_ == 0) {
            //			Log::warning() << "MapAllocator empty" << std::endl;
            next_ = (char*)addr_;
            left_ = length_;
        }
    }
    else {
        ASSERT(more_);
        more_->deallocate(addr);
        if (more_->count_ == 0 && more_->more_ == 0) {
            delete more_;
            more_ = 0;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
