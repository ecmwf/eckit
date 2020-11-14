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
#include <unistd.h>

#include "eckit/io/FDataSync.h"
#include "eckit/io/PooledFileDescriptor.h"

namespace eckit {

PooledFileDescriptor::PooledFileDescriptor(const PathName& path, bool readOnly) :
    path_(path), fd_(-1), readOnly_(readOnly) {}

PooledFileDescriptor::~PooledFileDescriptor() {
    close();
}

void PooledFileDescriptor::open() {
    if (readOnly_) {
        file_.reset(new PooledFile(path_));
        file_->open();
        fd_ = file_->fileno();
    }
    else {
        SYSCALL2(fd_ = ::open(path_.localPath(), O_RDWR | O_CREAT, 0777), path_);
    }
}

void PooledFileDescriptor::close() {
    if (fd_ < 0)
        return;

    if (readOnly_) {
        ASSERT(file_);
        file_->close();
        file_.reset();
    }
    else {
        SYSCALL(::close(fd_));
    }
    fd_ = -1;
}

ssize_t PooledFileDescriptor::read(void* buf, size_t nbyte) {
    if (readOnly_) {
        ASSERT(file_);
        return file_->read(buf, nbyte);
    }
    else {
        ssize_t len;
        SYSCALL(len = ::read(fd_, buf, nbyte));
        return len;
    }
}

ssize_t PooledFileDescriptor::write(const void* buf, size_t nbyte) {
    ASSERT(!readOnly_);

    ssize_t len;
    SYSCALL(len = ::write(fd_, buf, nbyte));
    return len;
}

void PooledFileDescriptor::sync() {
    ASSERT(!readOnly_);

    SYSCALL2(eckit::fdatasync(fd_), path_);
}

off_t PooledFileDescriptor::seek(off_t offset) {
    if (readOnly_) {
        ASSERT(file_);
        return file_->seek(offset);
    }
    else {
        off_t here;
        SYSCALL(here = ::lseek(fd_, offset, SEEK_SET));
        return here;
    }
}

off_t PooledFileDescriptor::seekEnd() {
    if (readOnly_) {
        ASSERT(file_);
        return file_->seekEnd();
    }
    else {
        off_t here;
        SYSCALL(here = ::lseek(fd_, 0, SEEK_END));
        return here;
    }
}

}  // namespace eckit
