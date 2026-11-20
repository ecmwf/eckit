// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/FileFlock.h"

#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#include <cerrno>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileFlock::FileFlock() : path_(""), create_(false), fd_(-1) {}

FileFlock::FileFlock(const LocalPathName& path, bool create) : path_(path), create_(create), fd_(-1) {}

FileFlock::FileFlock(FileFlock&& other) noexcept : path_(other.path_), create_(other.create_), fd_(other.fd_) {
    other.fd_ = -1;
}

FileFlock& FileFlock::operator=(FileFlock&& other) noexcept {
    if (this != &other) {
        close();
        path_     = other.path_;
        create_   = other.create_;
        fd_       = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

FileFlock::~FileFlock() {
    close();
}

void FileFlock::lockShared() {
    flock(LOCK_SH);
}

void FileFlock::lockExclusive() {
    flock(LOCK_EX);
}

bool FileFlock::tryLockShared() {
    return flock(LOCK_SH | LOCK_NB);
}

bool FileFlock::tryLockExclusive() {
    return flock(LOCK_EX | LOCK_NB);
}

void FileFlock::unlock() {
    if (fd_ != -1) {
        flock(LOCK_UN);
    }
}

void FileFlock::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool FileFlock::flock(int operation) {
    if (fd_ == -1) {
        fd_ = SYSCALL2(::open(path_.localPath(), O_RDONLY | O_CLOEXEC | (create_ ? O_CREAT : 0), 0644), path_);
    }

    while (::flock(fd_, operation) < 0) {
        int err = errno;
        if (err == EWOULDBLOCK) {
            return false;
        }
        if (err != EINTR) {
            throw FailedSystemCall("flock " + path_.path(), Here(), err);
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
