/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "eclib/machine.h"

#include "eclib/AutoLock.h"
#include "eclib/MappedArray.h"
#include "eclib/Padded.h"

inline unsigned long version(unsigned long long*) { return 1; }

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
	version_(version((T*)0)),
	headerSize_(sizeof(Header<T>)),
	elemSize_(sizeof(T))
{
}

template<class T>
void Header<T>::validate()
{
	ASSERT(version_    == version((T*)0));
	ASSERT(headerSize_ == sizeof(Header<T>));
	ASSERT(elemSize_   == sizeof(T));
}

template<class T>
MappedArray<T>::MappedArray(const PathName& path,unsigned long size):
	size_(size),
	sem_(path)
{

	AutoLock<Semaphore> lock(sem_);

	typedef Padded<Header<T>,4096> PaddedHeader;

	fd_ = ::open64(path.localPath(),O_RDWR | O_CREAT, 0777);
	if(fd_ < 0)
	{
		Log::error() << "open(" << path << ')' << Log::syserr << endl;
		throw FailedSystemCall("open");
	}

	struct stat64 s;
	SYSCALL(::stat64(path.localPath(),&s));

	bool initHeader = s.st_size < sizeof(PaddedHeader);

	off64_t length = size_ * sizeof(T) + sizeof(PaddedHeader);

	// Resize if needed

	if(s.st_size != length)
	{
		SYSCALL(ftruncate64(fd_,length));
		char buf1[sizeof(PaddedHeader)]; memset(buf1,0,sizeof(buf1));
		char buf2[sizeof(T)];            memset(buf2,0,sizeof(buf2));
	    SYSCALL(write(fd_,buf1,sizeof(buf1)));
		for(int i = 0; i < size_ ; i++)
			SYSCALL(write(fd_,buf2,sizeof(buf2)));
	}

	map_ = ::mmap64(0,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,0);
	if(map_ == 0) {
		Log::error() << "open(" << path << ',' << length << ')'
			<< Log::syserr << endl;
		throw FailedSystemCall("mmap");
	}

	// If first time in, init header

	if(initHeader)
		new(map_) PaddedHeader();
	else
		((PaddedHeader*)map_)->validate();

	array_ = (T*)(((char*)map_) + sizeof(PaddedHeader));

}

template<class T>
MappedArray<T>::~MappedArray()
{
	// Unmap here...
}

template<class T>
void MappedArray<T>::sync()
{
	int ret = fsync(fd_);
	while(ret < 0 && errno == EINTR)
		ret = fsync(fd_);
}
