// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eckit/container/KDTree.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/Crop.h"
#include "eckit/geo/cache/CropCache.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo::test {


/// Lists the files under a root by path, with their sizes (0 for directories)
inline std::map<std::string, std::uintmax_t> listing(const PathName& root) {
    std::map<std::string, std::uintmax_t> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root.asString())) {
        files[entry.path().string()] = entry.is_regular_file() ? entry.file_size() : 0;
    }
    return files;
}


/// Makes a root and the directories under it read-only, or writable again
inline void read_only(const PathName& root, bool yes) {
    using std::filesystem::perms;
    const auto p = perms::owner_read | perms::owner_exec | perms::group_read | perms::group_exec | perms::others_read |
                   perms::others_exec | (yes ? perms::none : perms::owner_write);

    std::filesystem::permissions(root.asString(), p);
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root.asString())) {
        if (entry.is_directory()) {
            std::filesystem::permissions(entry.path(), p);
        }
    }
}


/// Provisions a root with a k-d tree, as built elsewhere (the cache file layout)
inline void provision(const PathName& root, const std::string& uid, const std::vector<PointXYZ>& points) {
    const auto path = root / cache::SearchCache::file_cache().relative(uid);
    path.dirName().mkdir();

    std::vector<KDTreeMapped<search::Traits>::Value> values;
    for (size_t i = 0; i < points.size(); ++i) {
        values.emplace_back(points[i], i);
    }

    AutoUmask umask(0);
    KDTreeMapped<search::Traits>(path, points.size(), 0).build(values);
}


/// Provisions a root with a crop of a grid by an area, as computed elsewhere (any valid indices)
inline void provision(const PathName& root, const Grid& grid, const Area& area,
                      const cache::Crop::renumber_type& indices) {
    const auto path = root / cache::CropCache::file_cache().relative(cache::CropCache::key(grid, area));
    path.dirName().mkdir();

    AutoUmask umask(0);
    cache::Crop{indices}.write(path);
}


inline std::vector<PointXYZ> points_xyz(const Grid& grid) {
    const auto xyz = grid.to_xyz();

    std::vector<PointXYZ> points;
    for (size_t i = 0; i < xyz[0].size(); ++i) {
        points.emplace_back(xyz[0][i], xyz[1][i], xyz[2][i]);
    }
    return points;
}


/// If the coordinates of an unstructured grid are cached (for other processes, as registered by uid)
inline bool coordinates_cached(const Grid& grid) {
    std::unique_ptr<const spec::Spec> spec(GridSpecByUID::instance().get_spec(grid.uid()));
    return spec->has("cached_path");
}


}  // namespace eckit::geo::test
