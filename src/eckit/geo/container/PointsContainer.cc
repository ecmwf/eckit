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


#include "eckit/geo/container/PointsContainer.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo::container {


std::pair<std::vector<double>, std::vector<double>> PointsContainer::to_latlons(const std::vector<Point>& points) {
    std::pair<std::vector<double>, std::vector<double>> ll;
    ll.first.reserve(points.size());
    ll.second.reserve(points.size());

    for (const auto& p : points) {
        auto q = std::get<PointLonLat>(p);
        ll.first.emplace_back(q.lat);
        ll.second.emplace_back(q.lon);
    }

    return ll;
}


std::pair<std::vector<double>, std::vector<double>> PointsInstance::to_latlons() const {
    return PointsContainer::to_latlons(points_);
}


PointsLonLatReference::PointsLonLatReference(const std::vector<double>& longitudes,
                                             const std::vector<double>& latitudes) :
    longitudes(longitudes), latitudes(latitudes) {
    ASSERT(longitudes.size() == latitudes.size());
}

std::vector<Point> PointsLonLatReference::to_points() const {
    std::vector<Point> points;
    points.reserve(longitudes.size());

    for (size_t i = 0; i < size(); ++i) {
        points.emplace_back(PointLonLat{longitudes[i], latitudes[i]});
    }

    return points;
}


std::pair<std::vector<double>, std::vector<double>> PointsLonLatReference::to_latlons() const {
    return {latitudes, longitudes};
}


std::pair<std::vector<double>, std::vector<double>> PointsReference::to_latlons() const {
    return PointsContainer::to_latlons(points_);
}


}  // namespace eckit::geo::container
