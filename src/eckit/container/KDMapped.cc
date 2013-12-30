/*
 * (C) Copyright 1996-2013 ECMWF.
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


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


namespace eckit {

KDMapped::KDMapped(const PathName& path, size_t size, size_t metadata):
    path_(path),
    count_(0),
    size_(size),
    metadata_(metadata),
    header_(sizeof(size_t)),
    addr_(0),
    fd_(-1)
{
    int oflags = O_RDWR|O_CREAT;
    int mflags = PROT_READ|PROT_WRITE;
    
    if(size_ == 0) {
        oflags = O_RDONLY;
        mflags = PROT_READ;
    }

    SYSCALL(fd_ = ::open(path.localPath(),oflags,  0777));
    if(size_ == 0)
    {

        Stat::Struct s;
        SYSCALL(Stat::stat(path.localPath(),&s));
        size_ = s.st_size;

        int n;
        SYSCALL(n = ::read(fd_,&metadata_, header_));
        ASSERT(n == header_);
        lseek(fd_, 0, SEEK_SET);

    }
    else
    {
        char c = 0;

        size_ += header_ + metadata_;

        lseek(fd_, 0, SEEK_SET);
        SYSCALL(::write(fd_,&metadata_,header_));

        lseek(fd_, size_ - 1, SEEK_SET);
        SYSCALL(::write(fd_,&c,1));



    }
    
    Log::info() << "Mapping " << path << " " << size << std::endl;
    addr_ = ::mmap(0,size_, mflags, MAP_SHARED, fd_, 0 );
    if(addr_ == MAP_FAILED) {
        Log::error() << "open(" << path << ',' << size << ')'
                     << Log::syserr << std::endl;
        throw FailedSystemCall("mmap");
    }
}

KDMapped::~KDMapped()
{
    if(addr_)
        SYSCALL(munmap(addr_, size_));
    if(fd_ >= 0)
        SYSCALL(close(fd_));
}

// Warning, takes ownership of maps
KDMapped::KDMapped(const KDMapped& other):
    path_(other.path_),
    count_(other.count_),
    size_(other.size_),
    addr_(other.addr_),
    fd_(other.fd_),
    metadata_ (other.metadata_),
    header_(other.header_)
{
    const_cast<KDMapped&>(other).addr_ = 0;
    const_cast<KDMapped&>(other).fd_ = -1;
}

// Warning, takes ownership of maps
KDMapped& KDMapped::operator=(const KDMapped& other)
{
    path_ = other.path_;
    count_= other.count_;
    size_= other.size_;
    addr_= other.addr_;
    fd_ = other.fd_;

    metadata_ = other.metadata_;
    header_ = other.header_;

    const_cast<KDMapped&>(other).addr_ = 0;
    const_cast<KDMapped&>(other).fd_ = -1;

    return *this;
}

void KDMapped::setMetadata(const void *addr, size_t size) {
    ASSERT(size == metadata_);
    char *start = static_cast<char*>(addr_);
    ::memcpy(start + sizeof(size_t), addr, size);
}

void KDMapped::getMetadata(void *addr, size_t size) {
    ASSERT(size == metadata_);
    char *start = static_cast<char*>(addr_);
    ::memcpy(addr, start + sizeof(size_t), size);
}


} //namespace
