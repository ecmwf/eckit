// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/cache/Loader.h"


namespace eckit::cache::loader {


/**
 * @brief Content in (System V) shared memory, loaded once per host and attached by later processes
 * @details The segment, keyed by file, outlives the loading process (see unload, TempSharedMemory). Loading is
 * exclusive with creating the file (PathLock). Failing to use shared memory is an error, never a silent fallback. A
 * segment not matching the file as it is now (path, inode, mtime, size) is stale and replaced; attached processes are
 * unaffected.
 */
class SharedMemory : public Loader {
public:

    // -- Constructors

    SharedMemory(const PathName&, const Options&);

    // -- Destructor

    ~SharedMemory() override;

    // -- Class methods

    static MemoryUsage estimate(size_t size);

    /// If the file (as it is now) is loaded in shared memory
    static bool is_loaded(const PathName&);

    /// Remove the segment of a file (current or previous version); attached processes are unaffected
    static void unload(const PathName&);

    // -- Overridden methods

    const void* address() const override;
    size_t size() const override { return size_; }
    MemoryUsage footprint() const override;
    bool in_shared_memory() const override { return true; }

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
