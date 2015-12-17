/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015


#ifndef eckit_io_FilePool_h
#define eckit_io_FilePool_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"
#include "eckit/container/CacheLRU.h"

namespace eckit {
    class DataHandle;
    class PathName;
}

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Pool of a maximum number of open files.
/// Open files manipulated via DataHandle interface.
/// Handles are kept open as long as possible following LRU algorithm
/// Handles must be checked-out before usage, should be checked back in once writing finished.
/// No limit to number of checked out handles.
/// Handles are closed when purged from pool due to LRU or when pool is destroyed.

class FilePool : private eckit::NonCopyable {

public:

    FilePool(size_t capacity);

    ~FilePool();

    /// Checkout a DataHandle for use
    DataHandle& checkout(const PathName& path);

    /// Return a DataHandle after use
    void checkin(DataHandle& handle);

    /// Resize the max size of pool
    void resize( size_t size );

    /// Current size of pool
    size_t size() const;

    /// Returns max size of pool
    size_t capacity() const;

    /// Returns number of checked-out DataHandles
    size_t usage() const;

    /// Lists the contents of the Pool both in use and cached DataHandle's
    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& s, const FilePool& p) { p.print(s); return s; }

private:

    bool inUse(const PathName& path);

    void addToInUse(const PathName& path, DataHandle*);

    std::pair<PathName, DataHandle*> removeFromInUse(DataHandle*);

private:

    std::map< PathName, DataHandle* > inUse_;

    eckit::CacheLRU< PathName, DataHandle* > cache_;

    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif

