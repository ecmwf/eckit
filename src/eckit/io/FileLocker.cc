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

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileLocker.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FileLocker::FileLocker(int fd) :
    fd_(fd) {}

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
