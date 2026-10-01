// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <fcntl.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileLocker.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileLocker::FileLocker(int fd) : fd_(fd) {}

FileLocker::~FileLocker() {}

void FileLocker::lockExclusive(off_t off, off_t len) {
    lockRange(off, len, F_SETLKW, F_WRLCK);
}

void FileLocker::lockShared(off_t off, off_t len) {
    lockRange(off, len, F_SETLKW, F_RDLCK);
}

void FileLocker::unlock(off_t off, off_t len) {
    lockRange(off, len, F_SETLK, F_UNLCK);
}

void FileLocker::lockRange(off_t start, off_t len, int cmd, int type) {
    struct flock lock;

    lock.l_type   = type;
    lock.l_whence = SEEK_SET;
    lock.l_start  = start;
    lock.l_len    = len;

    SYSCALL(::fcntl(fd_, cmd, &lock));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
