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


#include "eckit/geo/grid/Reduced.h"

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::grid {


std::vector<Point> Reduced::to_points() const {
    std::vector<Point> points;
    points.reserve(size());

    const auto& lats = latitudes();
    ASSERT(lats.size() == ny());

    for (size_t j = 0; j < ny(); ++j) {
        const auto lons = longitudes(j);
        ASSERT(lons.size() == nx(j));

        const auto lat = lats.at(j);
        for (auto lon : lons) {
            points.emplace_back(PointLonLat{lon, lat});
        }
    }

    return points;
}


std::pair<std::vector<double>, std::vector<double>> Reduced::to_latlons() const {
    const auto N = size();

    std::pair<std::vector<double>, std::vector<double>> latlon;
    auto& lat = latlon.first;
    auto& lon = latlon.second;
    lat.reserve(N);
    lon.reserve(N);

    const auto& lats = latitudes();
    ASSERT(lats.size() == ny());

    for (size_t j = 0; j < ny(); ++j) {
        const auto lons = longitudes(j);

        lat.insert(lat.end(), lons.size(), lats.at(j));
        lon.insert(lon.end(), lons.begin(), lons.end());
    }

    ASSERT(lat.size() == N && lon.size() == N);
    return latlon;
}


const std::vector<size_t>& Reduced::nxacc() const {
    if (nxacc_.empty()) {
        nxacc_.resize(1 + ny());
        nxacc_.front() = 0;

        size_t j = 0;
        for (auto a = nxacc_.begin(), b = a + 1; b != nxacc_.end(); ++j, ++a, ++b) {
            *b = *a + nx(j);
        }

        ASSERT(nxacc_.back() == size());
    }

    return nxacc_;
}


}  // namespace eckit::geo::grid
