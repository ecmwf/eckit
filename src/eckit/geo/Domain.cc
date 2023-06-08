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


#include "eckit/geo/Domain.h"

#include <algorithm>
#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/util.h"
#include "eckit/geometry/Sphere.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


Domain::Domain(double north, double west, double south, double east) :
    north_(north), west_(west), south_(south), east_(east) {
    if (west_ != east_) {
        auto e = PointLonLat::normalise_angle_to_minimum(east, west);
        east_  = e == west_ ? (e + 360.) : e;
    }

    ASSERT_MSG(west_ <= east_ && east_ <= west_ + 360., "Domain: longitude range");
    ASSERT_MSG(-90. <= south_ && south_ <= north_ && north_ <= 90., "Domain: latitude range");
}


Domain::Domain() :
    Domain(90., 0., -90., 360.) {}


bool Domain::operator==(const Domain& other) const {
    return north_ == other.north_ && south_ == other.south_ && west_ == other.west_ && east_ == other.east_;
}


bool Domain::includesNorthPole() const {
    return north_ == NORTH_POLE;
}


bool Domain::includesSouthPole() const {
    return south_ == SOUTH_POLE;
}


bool Domain::isPeriodicWestEast() const {
    return west_ != east_ && west_ == PointLonLat::normalise_angle_to_minimum(east_, west_);
}


bool Domain::contains(double lat, double lon) const {
    return lat <= north_ && lat >= south_ && PointLonLat::normalise_angle_to_minimum(lon, west_) <= east_;
}


bool Domain::contains(const Domain& other) const {
    if (other.empty()) {
        return contains(other.south_, other.west_);
    }

    // check for West/East range (if non-periodic), then other's corners
    if (east_ - west_ < other.east_ - other.west_ || east_ < PointLonLat::normalise_angle_to_minimum(other.east_, west_)) {
        return false;
    }

    return contains(other.north_, other.west_) && contains(other.north_, other.east_) && contains(other.south_, other.west_) && contains(other.south_, other.east_);
}


bool Domain::empty() const {
    return types::is_approximately_equal(north_, south_) || types::is_approximately_equal(west_, east_);
}


double Domain::area(double radius) const {
    double lonf = isPeriodicWestEast() ? 1. : ((east_ - west_) / 360.);
    ASSERT(0. <= lonf && lonf <= 1.);

    const auto sn = std::sin(north_ * util::degree_to_radian);
    const auto ss = std::sin(south_ * util::degree_to_radian);

    double latf = 0.5 * (sn - ss);
    ASSERT(0. <= latf && latf <= 1.);

    return geometry::Sphere::area(radius) * latf * lonf;
}


}  // namespace eckit::geo
