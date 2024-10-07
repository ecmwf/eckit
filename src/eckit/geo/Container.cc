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


#include "eckit/geo/Container.h"


namespace eckit::geo {


std::pair<std::vector<double>, std::vector<double>> Container::to_latlons(const std::vector<Point>& points) {
    std::pair<std::vector<double>, std::vector<double>> ll;
    ll.first.reserve(points.size());
    ll.second.reserve(points.size());

    std::for_each(points.begin(), points.end(), [&ll](const auto& p) {
        auto q = std::get<PointLonLat>(p);
        ll.first.emplace_back(q.lat);
        ll.second.emplace_back(q.lon);
    });

    return ll;
}


}  // namespace eckit::geo
