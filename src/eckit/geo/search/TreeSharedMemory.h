// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>

#include "eckit/geo/search/TreeMappedFile.h"


namespace eckit::geo::search {


/**
 * @brief k-d tree in (System V) shared memory, loaded from its cache file (built as required)
 * @details The shared memory segment outlives the process, so other processes on the host attach to it instead of
 * loading the tree (see unload). If shared memory is not available (e.g. system limits), the cache file is used.
 */
class TreeSharedMemory final : public TreeMappedFile {
public:

    // -- Constructors

    TreeSharedMemory(const std::string& uid, size_t size);

    // -- Destructor

    ~TreeSharedMemory() override;

    // -- Methods

    bool in_shared_memory() const { return static_cast<bool>(segment_); }

    // -- Overridden methods

    Neighbour nearest_neighbour(const Point&) override;
    Neighbours k_nearest_neighbours(const Point&, size_t k) override;
    Neighbours find_in_sphere(const Point&, double radius) override;

    bool ready() override;
    void commit() override;

    void stats_print(std::ostream&, bool pretty) const override;
    void stats_reset() override;

    MemoryUsage footprint() const override;

    // -- Class methods

    /// Remove the shared memory segment of a cache file (processes attached to it are unaffected)
    static void unload(const PathName&);

private:

    // -- Types

    struct Segment;

    // -- Members

    std::unique_ptr<Segment> segment_;

    // -- Methods

    void load();
    void attach();

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
