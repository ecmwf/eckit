// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

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

    /// Cache file layout version (part of its path)
    static constexpr int VERSION = 1;

    LatitudeLongitude() = default;
    LatitudeLongitude(const std::vector<double>&, const std::vector<double>&);

    static const LatitudeLongitude& get(const uid_type&);
    static const LatitudeLongitude& set(const uid_type&, LatitudeLongitude&&);

    eckit::cache::MemoryUsage footprint() const override { return {2 * size() * sizeof(double), 0}; }

    void read(const PathName&) override;
    void write(const PathName&) const;

    PathName to_cached_path() const;

    /// Cache files of coordinates (latlon/VERSION/<uid>.ek), written locally for grids defined by their points
    static const eckit::cache::FileCache& file_cache();

    size_t size() const;

    const std::vector<double>& longitude() const { return lon_; }
    const std::vector<double>& latitude() const { return lat_; }

private:

    std::vector<double> lon_;
    std::vector<double> lat_;
};


}  // namespace eckit::geo::cache
