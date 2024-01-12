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


#include "eckit/geo/area/BoundingBox.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/Sphere.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


static constexpr std::array<double, 4> DEFAULT{90, 0, -90, 360};


static BoundingBox make_from_spec(const Spec& spec) {
    if (std::vector<double> area{DEFAULT[0], DEFAULT[1], DEFAULT[2], DEFAULT[3]}; spec.get("area", area)) {
        ASSERT_MSG(area.size() == 4, "BoundingBox: 'area' expected list of size 4");
        return {area[0], area[1], area[2], area[3]};
    }

    if (auto area(DEFAULT); spec.get("north", area[0]) && spec.get("west", area[1]) && spec.get("south", area[2]) &&
                            spec.get("east", area[3])) {
        return {area[0], area[1], area[2], area[3]};
    }

    if (!spec.has("north") && !spec.has("west") && !spec.has("south") && !spec.has("east")) {
        return {};
    }

    throw UserError("BoundingBox: expecting 'area'=north/west/south/east or ('north', 'west', 'south', 'east')");
}


BoundingBox::BoundingBox(const Spec& spec) :
    BoundingBox(make_from_spec(spec)) {}


BoundingBox::BoundingBox(double n, double w, double s, double e) :
    array{n, w, s, e} {
    if (west != east) {
        auto e = PointLonLat::normalise_angle_to_minimum(east, west);
        east   = e == west ? (e + 360.) : e;
    }

    ASSERT_MSG(west <= east && east <= west + 360., "BoundingBox: longitude range");
    ASSERT_MSG(-90. <= south && south <= north && north <= 90., "BoundingBox: latitude range");
}


BoundingBox::BoundingBox() :
    BoundingBox(DEFAULT[0], DEFAULT[1], DEFAULT[2], DEFAULT[3]) {}


bool BoundingBox::operator==(const BoundingBox& other) const {
    return north == other.north && south == other.south && west == other.west && east == other.east;
}


bool BoundingBox::isPeriodicWestEast() const {
    return west != east && west == PointLonLat::normalise_angle_to_minimum(east, west);
}


bool BoundingBox::contains(double lat, double lon) const {
    return lat <= north && lat >= south && PointLonLat::normalise_angle_to_minimum(lon, west) <= east;
}


bool BoundingBox::contains(const BoundingBox& other) const {
    if (other.empty()) {
        return contains(other.south, other.west);
    }

    // check for West/East range (if non-periodic), then other's corners
    if (east - west < other.east - other.west || east < PointLonLat::normalise_angle_to_minimum(other.east, west)) {
        return false;
    }

    return contains(other.north, other.west) && contains(other.north, other.east) &&
           contains(other.south, other.west) && contains(other.south, other.east);
}


bool BoundingBox::intersects(BoundingBox& other) const {
    auto n = std::min(north, other.north);
    auto s = std::max(south, other.south);

    bool intersectsSN = s <= n;
    if (!intersectsSN) {
        n = s;
    }

    if (isPeriodicWestEast() && other.isPeriodicWestEast()) {
        other = {n, other.west, s, other.east};
        return intersectsSN;
    }

    auto w = std::min(west, other.west);
    auto e = w;

    auto intersect = [](const BoundingBox& a, const BoundingBox& b, double& w, double& e) {
        bool p = a.isPeriodicWestEast();
        if (p || b.isPeriodicWestEast()) {
            w = (p ? b : a).west;
            e = (p ? b : a).east;
            return true;
        }

        auto ref = PointLonLat::normalise_angle_to_minimum(b.west, a.west);
        auto w_  = std::max(a.west, ref);
        auto e_  = std::min(a.east, PointLonLat::normalise_angle_to_minimum(b.east, ref));

        if (w_ <= e_) {
            w = w_;
            e = e_;
            return true;
        }

        return false;
    };

    bool intersectsWE = west <= other.west ? intersect(*this, other, w, e) || intersect(other, *this, w, e)
                                           : intersect(other, *this, w, e) || intersect(*this, other, w, e);

    ASSERT_MSG(w <= e, "BoundingBox::intersects: longitude range");
    other = {n, w, s, e};

    return intersectsSN && intersectsWE;
}


bool BoundingBox::empty() const {
    return types::is_approximately_equal(north, south) || types::is_approximately_equal(west, east);
}


double BoundingBox::area(double radius) const {
    double lonf = isPeriodicWestEast() ? 1. : ((east - west) / 360.);
    ASSERT(0. <= lonf && lonf <= 1.);

    const auto sn = std::sin(north * util::degree_to_radian);
    const auto ss = std::sin(south * util::degree_to_radian);

    double latf = 0.5 * (sn - ss);
    ASSERT(0. <= latf && latf <= 1.);

    return Sphere::area(radius) * latf * lonf;
}


BoundingBox BoundingBox::make(const BoundingBox&, const Projection&) {
    NOTIMP;
}


}  // namespace eckit::geo::area
