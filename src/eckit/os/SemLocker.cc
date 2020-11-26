/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <errno.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cstdio>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/SemLocker.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

struct sembuf _lock[] = {
    {0, 0, SEM_UNDO}, /* test */
    {0, 1, SEM_UNDO}, /* lock */
};

struct sembuf _unlock[] = {
    {0, -1, SEM_UNDO}, /* ulck */
};

}  // namespace


SemLocker::SemLocker(int sem, const PathName& path, int maxWaitLock) :
    sem_(sem), maxWaitLock_(maxWaitLock), path_(path) {
    int retry = 0;
    while (retry < maxWaitLock_) {
        if (semop(sem_, _lock, 2) < 0) {
            int save = errno;
            retry++;

            if (save == EINTR && retry < maxWaitLock_) {
                continue;
            }
            eckit::Log::warning() << "SharedMemoryLoader: Failed to acquire exclusive lock on " << path_ << " "
                                  << eckit::Log::syserr << std::endl;

            // sprintf(message,"ERR: sharedmem:semop:lock(%s)",path);
            if (retry >= maxWaitLock_) {
                std::ostringstream os;
                os << "Failed to acquire semaphore lock for " << path_;
                throw eckit::FailedSystemCall(os.str());
            }
            else {
                eckit::Log::warning() << "Sleeping for " << SLEEP << " seconds" << std::endl;
                ::sleep(SLEEP);
            }
        }
        else {
            break;
        }
    }
}

SemLocker::~SemLocker() {
    int retry = 0;
    while (retry < maxWaitLock_) {
        if (semop(sem_, _unlock, 1) < 0) {
            int save = errno;
            retry++;

            if (save == EINTR && retry < maxWaitLock_) {
                continue;
            }

            eckit::Log::warning() << "SharedMemoryLoader: Failed to realease exclusive lock on " << path_ << " "
                                  << eckit::Log::syserr << std::endl;

            if (retry >= maxWaitLock_) {
                std::ostringstream os;
                os << "Failed to release semaphore lock for " << path_;
                ASSERT_MSG(retry >= maxWaitLock_, os.str().c_str());
            }
            else {
                eckit::Log::warning() << "Sleeping for " << SLEEP << " seconds" << std::endl;
                sleep(SLEEP);
            }
        }
        else {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
