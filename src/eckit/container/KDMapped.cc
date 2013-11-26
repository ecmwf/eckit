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

KDMapped::KDMapped(const PathName& path, size_t size):
    path_(path),
    count_(0),
    size_(size),
    addr_(0),
    fd_(-1)
{

    SYSCALL(fd_ = ::open(path.localPath(), O_RDWR|O_CREAT,  0777));
    if(size_ == 0)
    {

        Stat::Struct s;
        SYSCALL(Stat::stat(path.localPath(),&s));
        size_ = s.st_size;
    }
    else
    {
        char c = 0;
        lseek(fd_, size_ - 1, SEEK_SET);
        SYSCALL(write(fd_,&c,1));
    }
    Log::info() << "Mapping " << path << " " << size << std::endl;
    addr_ = ::mmap(0,size_,PROT_READ|PROT_WRITE, MAP_SHARED, fd_, 0 );
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
    fd_(other.fd_)
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

    const_cast<KDMapped&>(other).addr_ = 0;
    const_cast<KDMapped&>(other).fd_ = -1;

    return *this;
}


} //namespace
