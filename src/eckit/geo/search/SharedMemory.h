// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/search/LoadedFile.h"


namespace eckit::geo::search {


/**
 * @brief k-d tree in (System V) shared memory, loaded from its cache file (built as required)
 * @details The shared memory segment outlives the process, so other processes on the host attach to it instead of
 * loading the k-d tree (see unload, and TempSharedMemory). If shared memory is not available (e.g. system limits), the
 * cache file is used.
 */
class SharedMemory : public LoadedFile {
public:

    // -- Constructors

    SharedMemory(const std::string& uid, size_t size);

    // -- Destructor

    ~SharedMemory() override;

    // -- Overridden methods

    MemoryUsage footprint() const override;

    // -- Class methods

    static MemoryUsage estimate(size_t size);

    /// If the cache file is loaded in shared memory
    static bool is_loaded(const PathName&);

    /// Remove the shared memory segment of a cache file (processes attached to it are unaffected)
    static void unload(const PathName&);

protected:

    // -- Constructors

    SharedMemory(const std::string& uid, size_t size, bool unload_at_exit);

private:

    // -- Types

    struct Segment;

    // -- Members

    const bool unload_at_exit_;
    std::unique_ptr<Segment> segment_;

    // -- Methods

    void attach();

    // -- Overridden methods

    char* load() override;
    void print(std::ostream&) const override;
};


/// k-d tree in (System V) shared memory, removed at exit (processes attached to it are unaffected)
class TempSharedMemory final : public SharedMemory {
public:

    // -- Constructors

    TempSharedMemory(const std::string& uid, size_t size);
};


}  // namespace eckit::geo::search
