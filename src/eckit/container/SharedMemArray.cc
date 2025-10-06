/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class T>
SharedMemArray<T>::SharedMemArray(const PathName& path, const std::string& shmName, size_t size) :
    sem_(path), size_(size), shmName_(shmName) {
    eckit::Log::debug<LibEcKit>() << "SharedMemArray semaphore path=" << path << ", size=" << size
                                  << ", shmName=" << shmName << std::endl;

    AutoLock<Semaphore> lock(sem_);

    typedef Padded<typename SharedMemArray<T>::Header, 4096> PaddedHeader;

    fd_ = ::shm_open(shmName_.c_str(), O_RDWR | O_CREAT, 0777);
    if (fd_ < 0) {
        Log::error() << "shm_open(" << shmName_ << ')' << Log::syserr << std::endl;
        throw FailedSystemCall("shm_open", Here());
    }

    Stat::Struct s;
    SYSCALL(Stat::fstat(fd_, &s));

    off_t length = size_ * sizeof(T) + sizeof(PaddedHeader);

    eckit::Log::debug<LibEcKit>() << "SharedMemArray fd_=" << fd_ << ", s.st_size=" << s.st_size
                                  << ", length=" << length << std::endl;

    // Resize if needed

    bool zero = false;
    if (length > s.st_size) {
        SYSCALL(::ftruncate(fd_, length));
        zero = true;
    }

    map_ = MMap::mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (map_ == MAP_FAILED) {
        Log::error() << "SharedMemArray name=" << shmName_ << " size=" << size
                     << " fails to mmap(0,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,0)" << Log::syserr << std::endl;
        throw FailedSystemCall("mmap", Here());
    }

    if (zero) {
        ::memset(map_, 0, sizeof(PaddedHeader) + size_ * sizeof(T));
        new (map_) PaddedHeader();
    }
    else {
        ((PaddedHeader*)map_)->validate();
    }

    array_ = (T*)(((char*)map_) + sizeof(PaddedHeader));
}

template <class T>
SharedMemArray<T>::~SharedMemArray() {
    // Unmap here...
}

template <class T>
void SharedMemArray<T>::sync() {
    //	int ret = fsync(fd_);
    //	while(ret < 0 && errno == EINTR)
    //		ret = fsync(fd_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
