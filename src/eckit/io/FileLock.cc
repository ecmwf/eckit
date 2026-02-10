/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/FileLock.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/os/AutoUmask.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static int openLock(const PathName& lockFile) {
    AutoUmask mask(0);
    int fd;
    lockFile.dirName().mkdir(0777);
    SYSCALL2(fd = ::open(lockFile.asString().c_str(), O_CREAT | O_RDWR, 0777), lockFile);
    return fd;
}


FileLock::FileLock(const PathName& lockFile) : lockFile_(lockFile), fd_(openLock(lockFile_)), locker_(fd_) {}


FileLock::~FileLock() {
    ::close(fd_);
}


void FileLock::lock() {
    locker_.lockExclusive();
}


void FileLock::unlock() {
    locker_.unlock();
    lockFile_.unlink(false);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
