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


#include "eckit/geo/geometry/BoundingBox.h"

#include <algorithm>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/geometry/Sphere.h"
#include "eckit/geo/util.h"


namespace eckit::geo::geometry {


BoundingBox::BoundingBox(double north, double west, double south, double east) :
    north_(north), west_(west), south_(south), east_(east) {
    if (west_ != east_) {
        auto e = util::normalise_longitude_to_minimum(east, west);
        east_  = e == west_ ? (e + 360.) : e;
    }

    ASSERT_MSG(west_ <= east_ && east_ <= west_ + 360., "BoundingBox: longitude range");
    ASSERT_MSG(-90. <= south_ && south_ <= north_ && north_ <= 90., "BoundingBox: latitude range");
}


BoundingBox::BoundingBox() :
    BoundingBox(90., 0., -90., 360.) {}


bool BoundingBox::operator==(const BoundingBox& other) const {
    return north_ == other.north_ && south_ == other.south_ && west_ == other.west_ && east_ == other.east_;
}


bool BoundingBox::isPeriodicWestEast() const {
    return west_ != east_ && west_ == util::normalise_longitude_to_minimum(east_, west_);
}


bool BoundingBox::contains(double lat, double lon) const {
    return lat <= north_ && lat >= south_ && util::normalise_longitude_to_minimum(lon, west_) <= east_;
}


bool BoundingBox::contains(const BoundingBox& other) const {
    if (other.empty()) {
        return contains(other.south_, other.west_);
    }

    // check for West/East range (if non-periodic), then other's corners
    if (east_ - west_ < other.east_ - other.west_ || east_ < util::normalise_longitude_to_minimum(other.east_, west_)) {
        return false;
    }

    return contains(other.north_, other.west_) && contains(other.north_, other.east_) && contains(other.south_, other.west_) && contains(other.south_, other.east_);
}


bool BoundingBox::intersects(BoundingBox& other) const {
    auto n = std::min(north_, other.north_);
    auto s = std::max(south_, other.south_);

    bool intersectsSN = s <= n;
    if (!intersectsSN) {
        n = s;
    }

    if (isPeriodicWestEast() && other.isPeriodicWestEast()) {
        other = {n, other.west_, s, other.east_};
        return intersectsSN;
    }

    auto w = std::min(west_, other.west_);
    auto e = w;

    auto intersect = [](const BoundingBox& a, const BoundingBox& b, double& w, double& e) {
        bool p = a.isPeriodicWestEast();
        if (p || b.isPeriodicWestEast()) {
            w = (p ? b : a).west_;
            e = (p ? b : a).east_;
            return true;
        }

        auto ref = util::normalise_longitude_to_minimum(b.west_, a.west_);
        auto w_  = std::max(a.west_, ref);
        auto e_  = std::min(a.east_, util::normalise_longitude_to_minimum(b.east_, ref));

        if (w_ <= e_) {
            w = w_;
            e = e_;
            return true;
        }

        return false;
    };

    bool intersectsWE = west_ <= other.west_ ? intersect(*this, other, w, e) || intersect(other, *this, w, e)
                                             : intersect(other, *this, w, e) || intersect(*this, other, w, e);

    ASSERT_MSG(w <= e, "BoundingBox::intersects: longitude range");
    other = {n, w, s, e};

    return intersectsSN && intersectsWE;
}


bool BoundingBox::empty() const {
    return util::is_approximately_equal(north_, south_) || util::is_approximately_equal(west_, east_);
}


double BoundingBox::area(double radius) const {
    double lonf = isPeriodicWestEast() ? 1. : ((east_ - west_) / 360.);
    ASSERT(0. <= lonf && lonf <= 1.);

    double latf = 0.5 * (std::sin(util::degrees_to_radians * north_) - std::sin(util::degrees_to_radians * south_));
    ASSERT(0. <= latf && latf <= 1.);

    return Sphere::area(radius) * latf * lonf;
}


}  // namespace eckit::geo::geometry
