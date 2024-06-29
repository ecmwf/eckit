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

#include "eckit/exception/Exceptions.h"


namespace eckit::geo::grid {


std::vector<Point> Reduced::to_points() const {
    std::vector<Point> points;
    points.reserve(size());

    const auto& lats = latitudes();
    ASSERT(lats.size() == nj());

    for (size_t j = 0; j < nj(); ++j) {
        const auto lons = longitudes(j);
        ASSERT(lons.size() == ni(j));

        const auto lat = lats.at(j);
        for (auto lon : lons) {
            points.emplace_back(PointLonLat{lon, lat});
        }
    }

    return points;
}


std::pair<std::vector<double>, std::vector<double>> Reduced::to_latlon() const {
    const auto N = size();

    std::pair<std::vector<double>, std::vector<double>> latlon;
    auto& lat = latlon.first;
    auto& lon = latlon.second;
    lat.reserve(N);
    lon.reserve(N);

    const auto& lats = latitudes();
    ASSERT(lats.size() == nj());

    for (size_t j = 0; j < nj(); ++j) {
        const auto lons = longitudes(j);

        lat.insert(lat.end(), lons.size(), lats.at(j));
        lon.insert(lon.end(), lons.begin(), lons.end());
    }

    ASSERT(lat.size() == N && lon.size() == N);
    return latlon;
}


Reduced::Reduced(const area::BoundingBox& bbox, Projection* projection) : Grid(bbox, projection) {}


const std::vector<size_t>& Reduced::niacc() const {
    if (niacc_.empty()) {
        niacc_.resize(1 + nj());
        niacc_.front() = 0;

        size_t j = 0;
        for (auto a = niacc_.begin(), b = a + 1; b != niacc_.end(); ++j, ++a, ++b) {
            *b = *a + ni(j);
        }

        ASSERT(niacc_.back() == size());
    }

    return niacc_;
}


}  // namespace eckit::geo::grid
