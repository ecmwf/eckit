/*
 * (C) Copyright 1996- ECMWF.
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

#include "eckit/container/CacheLRU.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/MutexCond.h"

namespace eckit {
class DataHandle;
class PathName;
}  // namespace eckit

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Pool of a maximum number of open files.
/// Open files manipulated via DataHandle interface.
/// Handles are kept open as long as possible following LRU algorithm
/// Handles must be checked-out before usage, should be checked back in once writing finished.
/// No limit to number of checked out handles.
/// Handles are closed when purged from pool due to LRU or when pool is destroyed.
///
/// @note this class is thread-safe
///
class FilePool : private eckit::NonCopyable {

public:

    FilePool(size_t capacity);

    ~FilePool();

    /// Checkout a DataHandle for use
    /// Ownership is passed from FilePool to the client
    /// @post DataHandle is marked in use and should not be closed by client
    /// @invariant If handle is in use, it assumes is from another thread and it will wait() for a checkin()
    DataHandle* checkout(const PathName& path);

    /// Return a DataHandle after use
    /// Ownership is passed back from the client to FilePool
    /// @post DataHandle is marked out of use and may now be closed by FilePool
    void checkin(DataHandle* handle);

    /// Remove a DataHandle from the pool
    /// @invariant If handle is in use, it assumes is from another thread and it will wait() for a checkin()
    bool remove(const PathName& path);

    /// Current size of pool
    size_t size() const;

    /// Resize pool capacity
    void capacity(size_t size);

    /// Returns max size of pool
    size_t capacity() const;

    /// Returns number of checked-out DataHandles
    size_t usage() const;

    /// Lists the contents of the Pool both in use and cached DataHandle's
    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& s, const FilePool& p) {
        p.print(s);
        return s;
    }

private:

    std::map<PathName, DataHandle*> inUse_;

    eckit::CacheLRU<PathName, DataHandle*> cache_;

    mutable eckit::MutexCond cond_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
