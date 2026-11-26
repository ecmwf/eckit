// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/PathLock.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/mutex.h"
#include "eckit/log/Log.h"


namespace eckit::cache {


// Per lock path, shared by its PathLock instances (never released, paths are few)
struct PathLock::State {
    util::recursive_mutex mutex;
    size_t depth = 0;
    int fd       = -1;  // lock file, while locked
};


PathLock::State& PathLock::state(const std::string& lock_path) {
    static util::recursive_mutex mutex;
    static auto* states = new std::map<std::string, std::unique_ptr<PathLock::State>>;

    util::lock_guard<util::recursive_mutex> lock(mutex);

    auto& s = (*states)[lock_path];
    if (!s) {
        s = std::make_unique<PathLock::State>();
    }
    return *s;
}


namespace {


// One lock file per file however its path is spelled (symbolic links), as fcntl locks are per file per process
PathName lock_file(const PathName& path) {
    const auto dir = path.dirName();
    return (dir.exists() ? dir.realName() : dir) / (path.baseName() + ".lock");
}


}  // namespace


PathLock::PathLock(const PathName& path, int timeout_s) :
    lock_path_(lock_file(path)), timeout_s_(timeout_s), state_(state(lock_path_.asString())) {}


PathLock::~PathLock() = default;


void PathLock::lock() {
    state_.mutex.lock();

    if (state_.depth++ > 0) {
        return;
    }

    // a lock file that can't be created (e.g. a read-only directory): nothing can be created there either
    const int fd = ::open(lock_path_.localPath(), O_CREAT | O_RDWR | O_CLOEXEC, PERMISSIONS);
    if (fd < 0) {
        Log::debug<LibEcKitCache>() << "PathLock: '" << lock_path_ << "' not available, locking threads only"
                                    << std::endl;
        return;
    }

    // used by all users (the umask is not changed, it is process-wide); not if of another user
    ::fchmod(fd, PERMISSIONS);

    struct flock lock{};
    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout_s_);

    int code = 0;
    while ((code = ::fcntl(fd, timeout_s_ > 0 ? F_SETLK : F_SETLKW, &lock)) != 0) {
        if (errno == EINTR) {
            continue;
        }

        // held by another process (F_SETLK only)
        if ((errno != EACCES && errno != EAGAIN) || std::chrono::steady_clock::now() >= deadline) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (code != 0) {
        const auto error = errno;
        ::close(fd);
        --state_.depth;
        state_.mutex.unlock();

        if (error == EACCES || error == EAGAIN) {
            throw CacheError("PathLock: '" + lock_path_.asString() + "' held by another process after " +
                                 std::to_string(timeout_s_) + "s",
                             Here());
        }

        errno = error;
        throw FailedSystemCall("PathLock: fcntl '" + lock_path_.asString() + "'", Here());
    }

    state_.fd = fd;
}


void PathLock::unlock() {
    ASSERT(state_.depth > 0);

    if (--state_.depth == 0 && state_.fd >= 0) {
        ::close(state_.fd);  // releases the lock
        state_.fd = -1;
    }

    state_.mutex.unlock();
}


}  // namespace eckit::cache
