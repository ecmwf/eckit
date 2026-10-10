// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/search/TreeMapped.h"
#include "eckit/geo/util/mutex.h"


namespace eckit {
class FileLock;
}


namespace eckit::geo::search {


/**
 * @brief k-d tree in a memory-mapped file, under the first writable root
 * @details Building is exclusive across threads and processes, into a temporary file renamed (atomically) on commit, so
 * a tree file is either complete or absent. An invalid tree file (e.g. of another layout) is rebuilt.
 */
class TreeMappedFile : public TreeMapped {
public:

    // -- Constructors

    TreeMappedFile(const std::string& uid, size_t size, const std::vector<PathName>& roots);

    // -- Destructor

    ~TreeMappedFile() override;

    // -- Methods

    const PathName& path() const { return path_; }

    // -- Overridden methods

    void build(std::vector<Value>&) override;
    void insert(const Value&) override;

    bool ready() override;
    void commit() override;

    void lock() override;
    void unlock() override;

    /// File-backed storage is shared (page cache)
    MemoryUsage footprint() const override { return {0, TreeMapped::footprint().memory()}; }

    // -- Class methods

    static PathName tree_path(const std::string& uid, const std::vector<PathName>& roots);

private:

    // -- Members

    const PathName path_;
    const PathName lock_path_;
    std::string tmp_;

    util::recursive_mutex& mutex_;
    std::unique_ptr<FileLock> file_lock_;

    // -- Methods

    void create();

    // -- Overridden methods

    void print(std::ostream&) const override;
};


class TreeMappedCacheFile final : public TreeMappedFile {
public:

    TreeMappedCacheFile(const std::string& uid, size_t size);

    static std::vector<PathName> roots();
};


class TreeMappedTempFile final : public TreeMappedFile {
public:

    TreeMappedTempFile(const std::string& uid, size_t size);

    static std::vector<PathName> roots();
};


}  // namespace eckit::geo::search
