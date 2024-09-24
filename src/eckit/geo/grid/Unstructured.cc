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


#include "eckit/geo/grid/Unstructured.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid {


Grid::iterator Unstructured::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, longitudes_, latitudes_)};
}


Grid::iterator Unstructured::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


std::vector<Point> Unstructured::to_points() const {
    auto n = size();
    std::vector<Point> points(n, PointLonLat{0., 0.});

    for (size_t i = 0; i < n; ++i) {
        points[i] = PointLonLat{longitudes_[i], latitudes_[i]};
    }

    return points;
}


Spec* Unstructured::spec(const std::string& name) {
    return SpecByUID::instance().get(name).spec();
}


Unstructured::Unstructured(const std::vector<Point>& points) {
    auto n = points.size();
    longitudes_.resize(n);
    latitudes_.resize(n);

    for (size_t i = 0; i < n; ++i) {
        auto& p        = std::get<PointLonLat>(points[i]);
        longitudes_[i] = p.lon;
        latitudes_[i]  = p.lat;
    }
}


void Unstructured::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", "unstructured");
    custom.set("uid", uid());
}


}  // namespace eckit::geo::grid
