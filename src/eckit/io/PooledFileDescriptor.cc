// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/PooledFileDescriptor.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "eckit/io/FDataSync.h"

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
    if (fd_ < 0) {
        return;
    }

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
    ssize_t len;
    SYSCALL(len = ::read(fd_, buf, nbyte));
    return len;
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
    off_t here;
    SYSCALL(here = ::lseek(fd_, offset, SEEK_SET));
    return here;
}

off_t PooledFileDescriptor::seekEnd() {
    if (readOnly_) {
        ASSERT(file_);
        return file_->seekEnd();
    }
    off_t here;
    SYSCALL(here = ::lseek(fd_, 0, SEEK_END));
    return here;
}

}  // namespace eckit
