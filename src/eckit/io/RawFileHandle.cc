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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/RawFileHandle.h"
#include "eckit/os/Stat.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RawFileHandle::print(std::ostream& s) const {
    s << "RawFileHandle[" << path_ << ']';
}

void RawFileHandle::encode(Stream& s) const {
    NOTIMP;
}

RawFileHandle::RawFileHandle(const std::string& path, bool overwrite) :
    path_(path), overwrite_(overwrite), fd_(-1) {}


RawFileHandle::~RawFileHandle() {
    if (fd_ != -1)
        close();
}

Length RawFileHandle::openForRead() {
    SYSCALL(fd_ = ::open(std::string(path_).c_str(), O_RDONLY));
    SYSCALL(::fcntl(fd_, F_SETFD, FD_CLOEXEC));
    struct stat st;
    SYSCALL(::fstat(fd_, &st));
    ASSERT(sizeof(st.st_size) == sizeof(long long));
    return st.st_size;
}

void RawFileHandle::openForWrite(const Length&) {
    if (overwrite_) {
        fd_ = SYSCALL(::open(std::string(path_).c_str(), O_WRONLY, 0777));
    }
    else {
        fd_ = SYSCALL(::open(std::string(path_).c_str(), O_WRONLY | O_CREAT, 0777));
    }
    SYSCALL(::fcntl(fd_, F_SETFD, FD_CLOEXEC));
}

void RawFileHandle::openForAppend(const Length&) {
    NOTIMP;
}

long RawFileHandle::read(void* buffer, long length) {
    long n;
    SYSCALL(n = ::read(fd_, buffer, length));
    return n;
}

long RawFileHandle::write(const void* buffer, long length) {
    long n;
    SYSCALL(n = ::write(fd_, buffer, length));
    return n;
}

void RawFileHandle::close() {
    /// @todo fsync
    SYSCALL(::close(fd_));
    fd_ = -1;
}

Offset RawFileHandle::position() {
    return ::lseek(fd_, 0, SEEK_CUR);
}

Offset RawFileHandle::seek(const Offset& o) {
    return ::lseek(fd_, o, SEEK_SET);
}

void RawFileHandle::skip(const Length& l) {
    ::lseek(fd_, l, SEEK_CUR);
}

Length RawFileHandle::size() {
    Stat::Struct info;
    SYSCALL(Stat::fstat(fd_, &info));
    return info.st_size;
}

Length RawFileHandle::estimate() {
    Stat::Struct info;
    SYSCALL(Stat::fstat(fd_, &info));
    return info.st_size;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
