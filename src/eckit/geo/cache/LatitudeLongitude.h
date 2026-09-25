// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/cache/RecordCache.h"


namespace eckit::geo::cache {


class LatitudeLongitude : public RecordCache {
public:

    using uid_type = geo::Grid::uid_type;

    LatitudeLongitude() = default;
    LatitudeLongitude(const std::vector<double>&, const std::vector<double>&);

    static const LatitudeLongitude& get(const uid_type&);
    static const LatitudeLongitude& set(const uid_type&, LatitudeLongitude&&);

    bytes_size_t footprint() const override { return 2 * size() * sizeof(double); }

    void read(const PathName&) override;
    void write(const PathName&) const;

    PathName to_cached_path() const;

    size_t size() const;

    const std::vector<double>& longitude() const { return lon_; }
    const std::vector<double>& latitude() const { return lat_; }

private:

    std::vector<double> lon_;
    std::vector<double> lat_;
};


}  // namespace eckit::geo::cache
