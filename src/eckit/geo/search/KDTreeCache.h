// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/search/KDTree.h"


namespace eckit::cache {
class Loader;
}


namespace eckit::geo::search {


/**
 * @brief k-d tree in a cache file, built once and then loaded by any eckit::cache loader
 * @details The file (KDMapped layout, see cache::SearchCache::file_cache) is queried in place wherever loaded, so
 * loading never rebuilds; an invalid file is rebuilt.
 */
class KDTreeCache final : public KDTree {
public:

    // -- Constructors

    KDTreeCache(const std::string& loader, const std::string& uid, size_t size);

    // -- Destructor

    ~KDTreeCache() override;

    // -- Methods

    /// Cache file (once built, or found)
    const PathName& path() const;

    /// Loaded cache file (once built)
    const eckit::cache::Loader& loader() const;

    /// If its cache file exists and is valid, so it is loaded rather than built (checked once, see build)
    bool found() const;

    // -- Overridden methods

    bool build(const Fill&, bool balanced = true) override;

    Neighbour nearest_neighbour(const Point&) override;
    Neighbours k_nearest_neighbours(const Point&, size_t k) override;
    Neighbours find_in_sphere(const Point&, double radius) override;

    void stats_print(std::ostream&, bool pretty) const override;
    void stats_reset() override;

    MemoryUsage footprint() const override;

    // -- Class methods

    /// If the loader loads a file's raw bytes (not memory: no file, nor stream: not the KDMapped layout)
    static bool loads(const std::string& loader);

    static MemoryUsage estimate(const std::string& loader, size_t size);

    /// Cache file size of a k-d tree of size points
    static size_t storage_size(size_t size);

    /// If a cache file is complete and of the expected layout
    static bool valid(const PathName&, size_t size);

    /// If loaded content is of the expected layout, as its cache file
    static bool valid(const char* content, size_t content_size, size_t size);

private:

    // -- Types

    /// Read-only k-d tree, over the loaded content
    struct Content;

    // -- Members

    const std::string loader_name_;
    const std::string uid_;
    mutable PathName path_;  ///< once found (see found_), or built
    mutable bool found_ = false;
    std::unique_ptr<eckit::cache::Loader> loader_;
    std::unique_ptr<Content> content_;

    // -- Methods

    Content& content();
    const Content& content() const;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::geo::search
