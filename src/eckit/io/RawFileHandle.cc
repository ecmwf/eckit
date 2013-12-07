/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/io/RawFileHandle.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void RawFileHandle::print(std::ostream& s) const
{
    s << "RawFileHandle[" << path_ << ']';
}

void RawFileHandle::encode(Stream& s) const
{
    NOTIMP;
}

RawFileHandle::RawFileHandle(const std::string& path,bool overwrite):
    path_(path),
    overwrite_(overwrite),
    fd_(-1)
{
}


RawFileHandle::~RawFileHandle()
{
    if(fd_ != -1)
        close();
}

Length RawFileHandle::openForRead()
{
    SYSCALL(fd_ = ::open(std::string(path_).c_str(), O_RDONLY|O_CLOEXEC));
    struct stat64 st;
    SYSCALL(::fstat64(fd_, &st));
    return st.st_size;
}

void RawFileHandle::openForWrite(const Length&)
{
    if(overwrite_) {
        fd_ = SYSCALL(::open(std::string(path_).c_str(), O_WRONLY|O_CLOEXEC,0777));
    }
    else {
        fd_ = SYSCALL(::open(std::string(path_).c_str(), O_WRONLY|O_CREAT|O_CLOEXEC,0777));
    }
}

void RawFileHandle::openForAppend(const Length&)
{
    NOTIMP;
}

long RawFileHandle::read(void* buffer,long length)
{
    long n;
    SYSCALL(n = ::read(fd_,buffer,length));
    return n;
}

long RawFileHandle::write(const void* buffer,long length)
{
    long n;
    SYSCALL(n = ::write(fd_,buffer,length));
    return n;
}

void RawFileHandle::close()
{
    //TODO: fsync
    SYSCALL(::close(fd_));
}

Offset RawFileHandle::position()
{
    return ::lseek(fd_, 0, SEEK_CUR);
}

Offset RawFileHandle::seek(const Offset& o)
{
    return ::lseek(fd_, o, SEEK_SET);
}

void RawFileHandle::skip(const Length& l)
{
    ::lseek(fd_, l, SEEK_CUR);
}

//-----------------------------------------------------------------------------

} // namespace eckit
