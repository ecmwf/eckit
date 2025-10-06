/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date   Oct 2016

#ifndef eckit_io_FileLock_h
#define eckit_io_FileLock_h

#include "eckit/io/FileLocker.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

class PathName;

//----------------------------------------------------------------------------------------------------------------------

class FileLock : public NonCopyable {

public:

    /// Constructor
    /// creates the lock file if needed
    FileLock(const PathName& lockFile);

    ~FileLock();

    void lock();
    void unlock();

private:

    int fd_;

    FileLocker locker_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
