// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <sys/types.h>

#include <memory>

#include "eckit/cache/Loader.h"


namespace eckit::cache::loader {


/**
 * @brief Content in (System V) shared memory, loaded once per host and user, and attached by later processes
 * @details The segment, keyed by file, outlives the loading process (see unload, TempSharedMemory). Loading is
 * exclusive with creating the file (PathLock). Failing to use shared memory is an error, never a silent fallback. A
 * segment not matching the file as it is now (path, inode, mtime, size) is stale and replaced; attached processes are
 * unaffected.
 */
class SharedMemory : public Loader {
public:

    // -- Constants

    /**
     * Permissions of the segments: private to their user. Anyone can calculate the key of a segment, so sharing between
     * users (e.g. 0644) would load content once per host, but trusting other users: one could create a segment first,
     * with a header matching the file, and its content would be used instead of the file's. For the same reason,
     * whatever its permissions, only a segment created by this user is attached; another user's is an error.
     */
    static constexpr int PERMISSIONS = 0600;

    // -- Constructors

    SharedMemory(const PathName&, const Options&);

    // -- Destructor

    ~SharedMemory() override;

    // -- Class methods

    /// Shared memory key of a file (by ftok with its modification time); not unique, as another file can share it
    static key_t key(const PathName&);

    /// Get the huge page size, 0 if unknown or not supported
    static size_t huge_page_size();

    static MemoryUsage estimate(size_t size);

    /// Remove the segment of a file (current or previous version); attached processes are unaffected
    static void unload(const PathName&);

    // -- Overridden methods

    const void* address() const override;
    size_t size() const override { return size_; }
    MemoryUsage footprint() const override;

protected:

    // -- Constructors

    SharedMemory(const PathName&, const Options&, bool unload_at_exit);

    // -- Overridden methods

    void print(std::ostream&) const override;

private:

    // -- Types

    struct Segment;

    // -- Members

    const size_t size_;
    const std::unique_ptr<Segment> segment_;  // after size_, as attaching reads it

    // -- Methods

    std::unique_ptr<Segment> attach(const Options&, bool unload_at_exit) const;
};


/// Content in (System V) shared memory, removed at exit of the loading process (attached processes are unaffected)
class TempSharedMemory final : public SharedMemory {
public:

    // -- Constructors

    TempSharedMemory(const PathName&, const Options&);

private:

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::cache::loader
