/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

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
