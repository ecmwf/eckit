// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>
#include <string>
#include <vector>

#include "eckit/container/KDTree.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/util/mutex.h"


namespace eckit {
class FileLock;
}


namespace eckit::geo::search {


/**
 * @brief k-d tree in its cache file (see LibEcKitGeo::cacheDir), memory-mapped
 * @details Building is exclusive across threads and processes, into a temporary file renamed (atomically) on commit, so
 * a cache file is either complete or absent. An invalid cache file (e.g. of another layout) is rebuilt.
 */
class MappedFile : public KDTree {
public:

    // -- Types

    using Container = KDTreeMapped<Traits>;

    // -- Constructors

    MappedFile(const std::string& uid, size_t size);

    // -- Destructor

    ~MappedFile() override;

    // -- Methods

    const PathName& path() const { return path_; }

    // -- Overridden methods

    void build(std::vector<Value>&) override;
    void insert(const Value&) override;

    Neighbour nearest_neighbour(const Point&) override;
    Neighbours k_nearest_neighbours(const Point&, size_t k) override;
    Neighbours find_in_sphere(const Point&, double radius) override;

    bool ready() override;
    void commit() override;

    void lock() override;
    void unlock() override;


    void stats_print(std::ostream&, bool pretty) const override;
    void stats_reset() override;

    /// File-backed storage is shared (page cache)
    MemoryUsage footprint() const override { return {0, mapped_ ? storage_size(size()) : 0}; }

    // -- Class methods

    static MemoryUsage estimate(size_t size) { return {0, storage_size(size)}; }

    /// Cache file size of a k-d tree of size points
    static size_t storage_size(size_t size);

    /// Cache file of a k-d tree, under a (writable) root
    static PathName kdtree_path(const std::string& uid, const PathName& root);

protected:

    // -- Methods

    /// Unmap the cache file (e.g. once loaded elsewhere)
    void close();

private:

    // -- Members

    const PathName path_;
    const PathName lock_path_;
    std::string tmp_;  // being built

    std::unique_ptr<Container> mapped_;
    size_t count_ = 0;

    util::recursive_mutex& mutex_;
    std::unique_ptr<FileLock> file_lock_;

    // -- Methods

    /// Map a file, created (for size() points) or existing
    void open(const PathName&, bool create);

    /// Create a temporary file to build into
    void create();

    /// Complete, and of the expected layout
    bool valid() const;

    Container& mapped();
    const Container& mapped() const;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
