// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/filesystem/LocalPathName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A flock() lock on a file. Unlike FileLock (fcntl record locks), the lock belongs to the open file, not to the
/// process: two FileFlocks on one file conflict even within one process, closing another descriptor of the file
/// does not release it, and a forked child shares it, so it is held until the last process holding it closes it.
///
/// Converting a held lock (exclusive to shared, or back) is not guaranteed to be atomic.
/// Creating the lock file occurs on first lock attempt, not on construction

class FileFlock {
public:  // methods

    FileFlock();
    explicit FileFlock(const LocalPathName& path, bool create = true);

    FileFlock(const FileFlock&)            = delete;
    FileFlock& operator=(const FileFlock&) = delete;

    FileFlock(FileFlock&& other) noexcept;
    FileFlock& operator=(FileFlock&& other) noexcept;

    ~FileFlock();

    void lockShared();
    void lockExclusive();

    bool tryLockShared();
    bool tryLockExclusive();

    void unlock();

    /// Close releases the lock unless a forked child still shares it.
    void close();

    const LocalPathName& path() const { return path_; }

private:  // methods

    bool flock(int operation);

private:  // members

    LocalPathName path_;
    bool create_;
    int fd_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
