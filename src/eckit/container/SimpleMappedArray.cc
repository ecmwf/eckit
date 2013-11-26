/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/mman.h>
#include <fcntl.h>

#include "eckit/eckit.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/os/Stat.h"

namespace eckit {

template<class T>
SimpleMappedArray<T>::SimpleMappedArray(const PathName& path,unsigned long size):
    size_(size)
{

    fd_ = ::open64(path.localPath(),O_RDWR | O_CREAT, 0777);
    if(fd_ < 0)
    {
        Log::error() << "open(" << path << ')' << Log::syserr << std::endl;
        throw FailedSystemCall("open");
    }

    Stat::Struct s;
    SYSCALL(Stat::stat(path.localPath(),&s));

    if(size_ == 0)
        size_ = off_t(s.st_size) / sizeof(T);

    ASSERT(size_);

    off_t length = off_t(size_) * sizeof(T);


    // Resize if needed

    if(s.st_size != length)
    {
        SYSCALL(::ftruncate(fd_,length));
        /*
           char buf1[sizeof(PaddedHeader)]; memset(buf1,0,sizeof(buf1));
           char buf2[sizeof(T)];            memset(buf2,0,sizeof(buf2));
           SYSCALL(write(fd_,buf1,sizeof(buf1)));
           for(int i = 0; i < size_ ; i++)
           SYSCALL(write(fd_,buf2,sizeof(buf2)));
           */
    }

    map_ = ::mmap(0,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,0);
    if(map_ == MAP_FAILED) {
        Log::error() << "open(" << path << ',' << length << ')'
            << Log::syserr << std::endl;
        throw FailedSystemCall("mmap");
    }

    array_ = static_cast<T*>(map_);

}

    template<class T>
SimpleMappedArray<T>::~SimpleMappedArray()
{
    // Unmap here...
}

    template<class T>
void SimpleMappedArray<T>::sync()
{
    int ret = fsync(fd_);
    while(ret < 0 && errno == EINTR)
        ret = fsync(fd_);
}

} //namespace
