// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * @brief Exclusive access to a path, across threads and processes (BasicLockable, recursive)
 * @details A per-path mutex excludes threads, then the outermost lock takes a file lock (fcntl, on
 * "<real directory>/<name>.lock", however the path is spelled) to exclude processes. If the lock file can't be created,
 * excluding threads suffices. Lock files are left in place, as removing them races with other processes. Waiting for
 * another process can be limited, so a process that hung holding the lock is reported; threads of a process always
 * wait.
 */
class PathLock {
public:

    // -- Constants

    /// Permissions of lock files: writable by all users, as locking requires it
    static constexpr int PERMISSIONS = 0666;

    // -- Constructors

    /// Waiting for another process for at most the timeout [s], then lock() throws CacheError; 0 waits without a limit
    explicit PathLock(const PathName&, int timeout_s = 0);

    PathLock(const PathLock&) = delete;
    PathLock(PathLock&&)      = delete;

    // -- Destructor

    ~PathLock();

    // -- Operators

    PathLock& operator=(const PathLock&) = delete;
    PathLock& operator=(PathLock&&)      = delete;

    // -- Methods

    void lock();
    void unlock();

private:

    // -- Types

    struct State;

    // -- Members

    const PathName lock_path_;
    const int timeout_s_;
    State& state_;

    // -- Class methods

    static State& state(const std::string& lock_path);
};


}  // namespace eckit::cache
