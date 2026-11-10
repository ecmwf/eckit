// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/RecordCache.h"


namespace eckit::cache {
class FileCache;
}


namespace eckit::geo::cache {


class LatitudeLongitude : public RecordCache {
public:

    using uid_type = geo::Grid::uid_type;

    /// Coordinates of a grid type, from its spec (default: from 'cached_path', or downloaded from 'url')
    using load_type = LatitudeLongitude (*)(const geo::Grid::Spec&);

    /// Cache file layout version (part of its path)
    static constexpr int VERSION = 1;

    LatitudeLongitude() = default;
    LatitudeLongitude(std::vector<double> lat, std::vector<double> lon);

    static const LatitudeLongitude& get(const uid_type&);
    static const LatitudeLongitude& set(const uid_type&, LatitudeLongitude&&);

    /// Register the coordinates loading of a grid type (a 'cached_path' takes precedence)
    static void register_load(const std::string& type, load_type);

    eckit::cache::MemoryUsage footprint() const override { return {2 * size() * sizeof(double), 0}; }

    void read(const PathName&) override;
    void write(const PathName&) const;

    PathName to_cached_path() const;
    std::optional<PathName> find_cached_path() const;

    /// Cache files of coordinates (latlon/VERSION/<uid>.ek), written locally for grids defined by their points
    static const eckit::cache::FileCache& file_cache();

    size_t size() const;

    const std::vector<double>& longitude() const { return lon_; }
    const std::vector<double>& latitude() const { return lat_; }

    Point point(size_t index) const { return PointLonLat{lon_.at(index), lat_.at(index)}; }

    /// Iterating over these coordinates, rather than getting them by uid on each point
    std::unique_ptr<geo::Grid::Iterator::State> make_state() const;

private:

    std::vector<double> lon_;
    std::vector<double> lat_;

    /// A cache file is valid if it reads as these coordinates
    eckit::cache::DiskCache::Valid valid() const;
};


}  // namespace eckit::geo::cache
