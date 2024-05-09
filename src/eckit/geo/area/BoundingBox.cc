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
#include <memory>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Sphere.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


static const BoundingBox GLOBE_PRIME{90., 0., -90., 360.};
static const BoundingBox GLOBE_ANTIPRIME{90., -180., -90., 180.};


BoundingBox* BoundingBox::make_global_prime() {
    return new BoundingBox(GLOBE_PRIME);
}


BoundingBox* BoundingBox::make_global_antiprime() {
    return new BoundingBox(GLOBE_ANTIPRIME);
}


void BoundingBox::spec(spec::Custom& custom) const {
    if (operator!=(GLOBE_PRIME)) {
        custom.set("area", std::vector<double>{north, west, south, east});
    }
}


BoundingBox* BoundingBox::make_from_spec(const Spec& spec) {
    const auto [n, w, s, e] = GLOBE_PRIME.deconstruct();

    if (std::vector<double> area{n, w, s, e}; spec.get("area", area)) {
        ASSERT_MSG(area.size() == 4, "BoundingBox: 'area' expected list of size 4");
        return new BoundingBox{area[0], area[1], area[2], area[3]};
    }

    std::array<double, 4> area{n, w, s, e};
    spec.get("north", area[0]);
    spec.get("south", area[2]);

    auto new_east = spec.get("west", area[1]) && !spec.has("east");
    auto new_west = spec.get("east", area[3]) && !spec.has("west");
    ASSERT(!new_east || !new_west);

    return new BoundingBox{area[0], new_west ? area[3] - 360. : area[1], area[2], new_east ? area[1] + 360. : area[3]};
}


BoundingBox::BoundingBox(const Spec& spec) : BoundingBox(*std::unique_ptr<BoundingBox>(make_from_spec(spec))) {}


BoundingBox::BoundingBox(double n, double w, double s, double e) : array{n, w, s, e} {
    ASSERT_MSG(types::is_approximately_lesser_or_equal(-90., south), "BoundingBox: latitude range (-90 <= south)");
    ASSERT_MSG(types::is_approximately_lesser_or_equal(south, north), "BoundingBox: latitude range (south <= north)");
    ASSERT_MSG(types::is_approximately_lesser_or_equal(north, 90.), "BoundingBox: latitude range (north <= 90)");

    if (!types::is_approximately_equal(west, east)) {
        auto e = PointLonLat::normalise_angle_to_minimum(east, west);
        east   = types::is_approximately_equal(e, west) ? (e + 360.) : e;
    }

    ASSERT_MSG(types::is_approximately_lesser_or_equal(west, east), "BoundingBox: longitude range (west <= east)");
    ASSERT_MSG(types::is_approximately_lesser_or_equal(east, west + 360.),
               "BoundingBox: longitude range (east <= west + 360)");
}


BoundingBox::BoundingBox() : BoundingBox(GLOBE_PRIME) {}


bool BoundingBox::operator==(const BoundingBox& other) const {
    return types::is_approximately_equal(north, other.north, PointLonLat::EPS)
           && types::is_approximately_equal(south, other.south, PointLonLat::EPS)
           && types::is_approximately_equal(west, other.west, PointLonLat::EPS)
           && types::is_approximately_equal(east, other.east, PointLonLat::EPS);
}


bool BoundingBox::isPeriodicWestEast() const {

    return west != east
           && types::is_approximately_equal(west, PointLonLat::normalise_angle_to_minimum(east, west),
                                            PointLonLat::EPS);
}


bool BoundingBox::containsNorthPole() const {
    static const auto NORTH_POLE = PointLonLat::make(0., 90.);
    return points_equal({0., north}, NORTH_POLE);
}


bool BoundingBox::containsSouthPole() const {
    static const auto SOUTH_POLE = PointLonLat::make(0., -90.);
    return points_equal({0., south}, SOUTH_POLE);
}


bool BoundingBox::contains(const PointLonLat& p) const {
    return p.lat <= north && p.lat >= south && PointLonLat::normalise_angle_to_minimum(p.lon, west) <= east;
}


bool BoundingBox::contains(const BoundingBox& other) const {
    if (other.empty()) {
        return contains({other.south, other.west});
    }

    // check for West/East range (if non-periodic), then other's corners
    if (east - west < other.east - other.west || east < PointLonLat::normalise_angle_to_minimum(other.east, west)) {
        return false;
    }

    return contains({other.north, other.west}) && contains({other.north, other.east})
           && contains({other.south, other.west}) && contains({other.south, other.east});
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

    const auto sn = std::sin(north * util::DEGREE_TO_RADIAN);
    const auto ss = std::sin(south * util::DEGREE_TO_RADIAN);

    double latf = 0.5 * (sn - ss);
    ASSERT(0. <= latf && latf <= 1.);

    return Sphere::area(radius) * latf * lonf;
}


BoundingBox BoundingBox::calculate(const BoundingBox&, const Projection&) {
    NOTIMP;
}


}  // namespace eckit::geo::area
