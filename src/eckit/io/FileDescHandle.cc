/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileDescHandle.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void FileDescHandle::print(std::ostream& s) const {
    s << "FileDescHandle[fd=" << fd_ << ']';
}

void FileDescHandle::encode(Stream& s) const {
    NOTIMP;
}

FileDescHandle::FileDescHandle(int fd, bool close) :
    fd_(fd), close_(close) {}

FileDescHandle::~FileDescHandle() {
    if (fd_ != -1) {
        close();
    }
}

Length FileDescHandle::openForRead() {
    return 0;
}

void FileDescHandle::openForWrite(const Length&) {}

void FileDescHandle::openForAppend(const Length&) {}

long FileDescHandle::read(void* buffer, long length) {
    return ::read(fd_, buffer, length);
}

long FileDescHandle::write(const void* buffer, long length) {
    return ::write(fd_, buffer, length);
}

void FileDescHandle::close() {
    if (close_ && (fd_ != -1)) {
        SYSCALL(::close(fd_));
        fd_ = -1;
    }
}

Offset FileDescHandle::position() {
    off_t pos;
    SYSCALL(pos = ::lseek(fd_, 0, SEEK_CUR));
    return pos;
}

Offset FileDescHandle::seek(const Offset& o) {
    off_t pos;
    SYSCALL(pos = ::lseek(fd_, o, SEEK_SET));
    return pos;
}

void FileDescHandle::skip(const Length& l) {
    SYSCALL(::lseek(fd_, l, SEEK_CUR));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
