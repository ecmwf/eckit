// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/filesystem/PathName.h"


namespace eckit::cache {


/**
 * @brief Exclusive access to a path, across threads and processes (BasicLockable, recursive)
 * @details A per-path mutex excludes threads, then the outermost lock takes a file lock (fcntl, on
 * "<real directory>/<name>.lock", however the path is spelled) to exclude processes. If the lock file can't be created,
 * excluding threads suffices. Lock files are left in place, as removing them races with other processes.
 */
class PathLock {
public:

    // -- Constructors

    explicit PathLock(const PathName&);

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

    const PathName& lock_path() const { return lock_path_; }

private:

    // -- Types

    struct State;

    // -- Members

    const PathName lock_path_;
    State& state_;

    // -- Class methods

    static State& state(const std::string& lock_path);
};


}  // namespace eckit::cache
