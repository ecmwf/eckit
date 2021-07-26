/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "KDMapped.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/os/Stat.h"


#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>  // for memcpy

#include "eckit/memory/MMap.h"


namespace eckit {


KDMapped::KDMapped(const PathName& path, size_t itemCount, size_t itemSize, size_t metadataSize) :
    path_(path), header_(itemCount, itemSize, metadataSize), size_(0), base_(0), root_(0), addr_(0), fd_(-1) {

    int oflags = O_RDWR | O_CREAT;
    int mflags = PROT_READ | PROT_WRITE;

    if (itemCount == 0) {
        oflags = O_RDWR;
        // mflags = PROT_READ;
    }

    size_t base;

    SYSCALL(fd_ = ::open(path.localPath(), oflags, 0777));
    if (itemCount == 0) {
        readonly_ = true;
        Stat::Struct s;
        SYSCALL(Stat::stat(path.localPath(), &s));
        size_ = s.st_size;


        int n;
        SYSCALL(n = ::read(fd_, &header_, sizeof(header_)));
        ASSERT(n == sizeof(header_));
        lseek(fd_, 0, SEEK_SET);

        root_ = 1;

        ASSERT(header_.headerSize_ == sizeof(header_));

        base   = ((header_.headerSize_ + header_.metadataSize_ + header_.itemSize_ - 1) / header_.itemSize_) * header_.itemSize_;
        count_ = header_.itemCount_;
    }
    else {
        readonly_ = false;
        base      = ((header_.headerSize_ + header_.metadataSize_ + header_.itemSize_ - 1) / header_.itemSize_) * header_.itemSize_;

        char c = 0;

        size_ = base + (itemCount + 1) * itemSize;

        lseek(fd_, 0, SEEK_SET);
        SYSCALL(::write(fd_, &header_, sizeof(header_)));

        lseek(fd_, size_ - 1, SEEK_SET);
        SYSCALL(::write(fd_, &c, 1));
    }

    addr_ = MMap::mmap(0, size_, mflags, MAP_SHARED, fd_, 0);
    if (addr_ == MAP_FAILED) {
        Log::error() << "open(" << path << ')' << Log::syserr << std::endl;
        throw FailedSystemCall("mmap");
    }

    base_ = reinterpret_cast<char*>(addr_) + base;
}

KDMapped::~KDMapped() {
    if (addr_)
        SYSCALL(munmap(addr_, size_));
    if (fd_ >= 0)
        SYSCALL(close(fd_));
}

// Warning, takes ownership of maps
KDMapped::KDMapped(const KDMapped& other) :
    path_(other.path_),
    header_(other.header_),
    count_(other.count_),
    size_(other.size_),
    base_(other.base_),
    root_(other.root_),
    addr_(other.addr_),
    fd_(other.fd_) {
    const_cast<KDMapped&>(other).addr_ = 0;
    const_cast<KDMapped&>(other).fd_   = -1;
}

// Warning, takes ownership of maps
KDMapped& KDMapped::operator=(const KDMapped& other) {
    path_  = other.path_;
    count_ = other.count_;
    size_  = other.size_;
    addr_  = other.addr_;
    fd_    = other.fd_;
    root_  = other.root_;

    header_ = other.header_;
    base_   = other.base_;

    const_cast<KDMapped&>(other).addr_ = 0;
    const_cast<KDMapped&>(other).fd_   = -1;

    return *this;
}

void KDMapped::setMetadata(const void* addr, size_t size) {
    ASSERT(size == header_.metadataSize_);
    char* start = static_cast<char*>(addr_);
    ::memcpy(start + sizeof(header_), addr, size);
}

void KDMapped::getMetadata(void* addr, size_t size) {
    ASSERT(size == header_.metadataSize_);
    char* start = static_cast<char*>(addr_);
    ::memcpy(addr, start + sizeof(header_), size);
}


}  // namespace eckit
