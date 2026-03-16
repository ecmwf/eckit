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
#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/AutoUmask.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static int openLock(const PathName& lockFile) {
    AutoUmask mask(0);
    int fd = 0;
    lockFile.dirName().mkdir(0777);
    SYSCALL2(fd = ::open(lockFile.asString().c_str(), O_CREAT | O_RDWR, 0777), lockFile);
    return fd;
}


FileLock::FileLock(const PathName& lockFile, bool unlink_at_destruction) :
    lockFile_(unlink_at_destruction ? lockFile : ""), fd_(openLock(lockFile)), locker_(fd_) {}


FileLock::~FileLock() {
    ::close(fd_);
    if (!lockFile_.asString().empty()) {
        try {
            lockFile_.unlink(false);
        }
        catch (const FailedSystemCall& e) {
            Log::warning() << "Failed to unlink lock file " << lockFile_ << " -- " << e.what() << std::endl;
        }
    }
}


void FileLock::lock() {
    locker_.lockExclusive();
}


void FileLock::unlock() {
    locker_.unlock();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
