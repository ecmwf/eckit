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

#include <utility>
#include <type_traits>
#include <variant>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointLonLatR.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid {


Grid::iterator Unstructured::cbegin() const {
    return iterator{new geo::iterator::Unstructured(*this, 0, longitudes(), latitudes())};
}


Grid::iterator Unstructured::cend() const {
    return iterator{new geo::iterator::Unstructured(*this)};
}


Unstructured::Unstructured(const Spec& spec) :
    longitudes_(spec.get_double_vector("longitudes")), latitudes_(spec.get_double_vector("latitudes")) {
    ASSERT(longitudes_.size() == latitudes_.size());
}


Unstructured::Unstructured(const std::vector<double>& longitudes, const std::vector<double>& latitudes) :
    longitudes_(longitudes), latitudes_(latitudes) {
    ASSERT(longitudes_.size() == latitudes_.size());
}


std::vector<size_t> Unstructured::shape() const {
    return {longitudes_.size()};
}


Unstructured::Unstructured() = default;


Grid::BoundingBox* Unstructured::calculate_bbox() const {
    return new BoundingBox;
}


std::vector<Point> Unstructured::to_points() const {
    const auto& lons = longitudes();
    const auto& lats = latitudes();
    ASSERT(lons.size() == lats.size());

    std::vector<Point> pts;
    pts.reserve(size());

    for (size_t i = 0; i < lons.size(); ++i) {
        pts.emplace_back(PointLonLat{lons[i], lats[i]});
    }

    return pts;
}


std::pair<std::vector<double>, std::vector<double> > Unstructured::to_latlons() const {
    return {latitudes(), longitudes()};
}


Grid::Spec* Unstructured::spec(const std::string& uid) {
    if (!GridSpecByUID::instance().exists(uid)) {
        throw exception::GridUnknownError("Unstructured: unknown grid uid '" + uid + "'", Here());
    }

    return GridSpecByUID::instance().get(uid).spec();
}


void Unstructured::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", "unstructured");
    custom.set("uid", uid());
}


}  // namespace eckit::geo::grid
