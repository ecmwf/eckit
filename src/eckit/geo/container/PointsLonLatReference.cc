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


#include "eckit/geo/container/PointsLonLatReference.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo::container {


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


}  // namespace eckit::geo::container
