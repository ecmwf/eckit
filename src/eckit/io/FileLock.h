// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date   Oct 2016

#ifndef eckit_io_FileLock_h
#define eckit_io_FileLock_h

#include "eckit/filesystem/PathName.h"
#include "eckit/io/FileLocker.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileLock {

public:

    /// Constructor
    /// creates the lock file if needed
    explicit FileLock(const PathName& lockFile, bool unlink_at_destruction = false);

    FileLock(const FileLock&)            = delete;
    FileLock& operator=(const FileLock&) = delete;
    FileLock(FileLock&&)                 = delete;
    FileLock& operator=(FileLock&&)      = delete;

    ~FileLock();

    void lock();
    void unlock();

private:

    const PathName lockFile_;
    int fd_;
    FileLocker locker_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
