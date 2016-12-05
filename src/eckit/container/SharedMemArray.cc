/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template<class T>
unsigned long shared_mem_array_version(T*) { return 1; }

template<class T>
struct Header {
	uint32_t version_;
	uint32_t headerSize_;
	uint32_t elemSize_;
	Header();
	void validate();
};

template<class T>
Header<T>::Header():
    version_(shared_mem_array_version((T*)0)),
	headerSize_(sizeof(Header<T>)),
	elemSize_(sizeof(T))
{
}

template<class T>
void Header<T>::validate()
{
    ASSERT(version_    == shared_mem_array_version((T*)0));
	ASSERT(headerSize_ == sizeof(Header<T>));
	ASSERT(elemSize_   == sizeof(T));
}

int tounderscore(int p) {
    return p == '/' ? '_' : p;
}

template<class T>
SharedMemArray<T>::SharedMemArray(const PathName& path, size_t size):
	sem_(path),
    size_(size)    
{
    eckit::Log::info() << "SharedMemArray path=" << path << ", size=" << size << std::endl;

	AutoLock<Semaphore> lock(sem_);

	typedef Padded<Header<T>,4096> PaddedHeader;

    std::string r = path.asString();
    std::transform(r.begin(), r.end(), r.begin(), static_cast < int(*)(int) > (tounderscore));

    eckit::Log::info() << "SharedMemArray realpath=" << r << std::endl;


    fd_ = ::shm_open(r.c_str(), O_RDWR | O_CREAT, 0777);
	if(fd_ < 0)
	{
        Log::error() << "shm_open(" << r << ')' << Log::syserr << std::endl;
        throw FailedSystemCall("open",Here());
	}

    Stat::Struct s;
    SYSCALL(Stat::fstat(fd_, &s));

    bool initHeader = s.st_size < static_cast<long int>( sizeof(PaddedHeader) );

	off_t length = size_ * sizeof(T) + sizeof(PaddedHeader);

	// Resize if needed

	if(s.st_size != length)
	{
        SYSCALL(::ftruncate(fd_,length));
		char buf1[sizeof(PaddedHeader)]; memset(buf1,0,sizeof(buf1));
		char buf2[sizeof(T)];            memset(buf2,0,sizeof(buf2));
	    SYSCALL(write(fd_,buf1,sizeof(buf1)));
        for(size_t i = 0; i < size_ ; i++)
			SYSCALL(write(fd_,buf2,sizeof(buf2)));
	}

    map_ = ::mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd_, 0);
    if(map_ == MAP_FAILED)
    {
        Log::error() << "SharedMemArray path=/dev/shm/" << path << " size=" << size
                     << " fails to mmap(0,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,0)"
                     << Log::syserr << std::endl;
        throw FailedSystemCall("mmap",Here());
    }

	// If first time in, init header

	if(initHeader)
		new(map_) PaddedHeader();
	else
		((PaddedHeader*)map_)->validate();

	array_ = (T*)(((char*)map_) + sizeof(PaddedHeader));

}

template<class T>
SharedMemArray<T>::~SharedMemArray()
{
	// Unmap here...
}

template<class T>
void SharedMemArray<T>::sync()
{
//	int ret = fsync(fd_);
//	while(ret < 0 && errno == EINTR)
//		ret = fsync(fd_);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

