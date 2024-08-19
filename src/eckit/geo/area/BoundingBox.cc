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
#include <memory>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/geometry/Sphere.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::area {


const BoundingBox BOUNDING_BOX_DEFAULT;


static inline bool is_approximately_equal(BoundingBox::value_type a, BoundingBox::value_type b) {
    return types::is_approximately_equal(a, b, PointLonLat::EPS);
}


BoundingBox* BoundingBox::make_global_prime() {
    return new BoundingBox{PointLonLat::RIGHT_ANGLE, 0., -PointLonLat::RIGHT_ANGLE, PointLonLat::FULL_ANGLE};
}


BoundingBox* BoundingBox::make_global_antiprime() {
    return new BoundingBox{PointLonLat::RIGHT_ANGLE, -PointLonLat::FLAT_ANGLE, -PointLonLat::RIGHT_ANGLE,
                           PointLonLat::FLAT_ANGLE};
}


void BoundingBox::fill_spec(spec::Custom& custom) const {
    if (operator!=(BOUNDING_BOX_DEFAULT)) {
        custom.set("area", "bounding-box");
        custom.set("bounding-box", std::vector<double>{north, west, south, east});
    }
}


BoundingBox* BoundingBox::make_from_spec(const Spec& spec) {
    auto [n, w, s, e] = BOUNDING_BOX_DEFAULT.deconstruct();

    if (std::vector<double> area{n, w, s, e}; spec.get("area", area)) {
        ASSERT_MSG(area.size() == 4, "BoundingBox: 'area' expected list of size 4");
        return make_from_area(area[0], area[1], area[2], area[3]);
    }

    spec.get("north", n);
    spec.get("south", s);

    if (spec.get("west", w) && !spec.has("east")) {
        e = w + PointLonLat::FULL_ANGLE;
    }

    if (spec.get("east", e) && !spec.has("west")) {
        w = e - PointLonLat::FULL_ANGLE;
    }

    return make_from_area(n, w, s, e);
}


BoundingBox* BoundingBox::make_from_area(value_type n, value_type w, value_type s, value_type e) {
    // set latitudes inside usual range (not a normalisation like PointLonLat::make)
    if (n > NORTH_POLE.lat || is_approximately_equal(n, NORTH_POLE.lat)) {
        n = NORTH_POLE.lat;
    }

    if (s < SOUTH_POLE.lat || is_approximately_equal(s, SOUTH_POLE.lat)) {
        s = SOUTH_POLE.lat;
    }

    // normalise west in [min, min + 2 pi[, east in [west, west + 2 pi[
    constexpr auto min = BOUNDING_BOX_NORMALISE_WEST;
    const auto same    = is_approximately_equal(w, e);

    w = is_approximately_equal(w, min) || is_approximately_equal(w, min + PointLonLat::FULL_ANGLE)
            ? min
            : PointLonLat::normalise_angle_to_minimum(w, min);

    auto a = PointLonLat::normalise_angle_to_minimum(e, w);
    e      = same ? w : is_approximately_equal(w, a) ? (w + PointLonLat::FULL_ANGLE) : a;

    return new BoundingBox{n, w, s, e};
}


BoundingBox::BoundingBox(const Spec& spec) : BoundingBox(*std::unique_ptr<BoundingBox>(make_from_spec(spec))) {}


BoundingBox::BoundingBox(double n, double w, double s, double e) : array{n, w, s, e} {
    // normalise east in [west, west + 2 pi[
    auto a        = PointLonLat::normalise_angle_to_minimum(e, w);
    operator[](3) = is_approximately_equal(w, e) ? w : is_approximately_equal(w, a) ? (w + PointLonLat::FULL_ANGLE) : a;

    ASSERT(south <= north);
    ASSERT(west <= east);
}


BoundingBox::BoundingBox() : BoundingBox(*std::unique_ptr<BoundingBox>(make_global_prime())) {}


bool BoundingBox::global() const {
    return periodic() && contains(NORTH_POLE) && contains(SOUTH_POLE);
}


bool BoundingBox::periodic() const {
    return west != east && is_approximately_equal(west, PointLonLat::normalise_angle_to_minimum(east, west));
}


bool BoundingBox::contains(const PointLonLat& p) const {
    // NOTE: latitudes < -90 or > 90 are not considered
    if (is_approximately_equal(p.lat, NORTH_POLE.lat)) {
        return is_approximately_equal(p.lat, north);
    }

    if (is_approximately_equal(p.lat, SOUTH_POLE.lat)) {
        return is_approximately_equal(p.lat, south);
    }

    if ((south < p.lat && p.lat < north) || is_approximately_equal(p.lat, north)
        || is_approximately_equal(p.lat, south)) {
        return PointLonLat::normalise_angle_to_minimum(p.lon, west) <= east;
    }

    return false;
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

    if (periodic() && other.periodic()) {
        other = {n, other.west, s, other.east};
        return intersectsSN;
    }

    auto w = std::min(west, other.west);
    auto e = w;

    auto intersect = [](const BoundingBox& a, const BoundingBox& b, double& w, double& e) {
        bool p = a.periodic();
        if (p || b.periodic()) {
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
    return is_approximately_equal(north, south) || is_approximately_equal(west, east);
}


bool bounding_box_equal(const BoundingBox& a, const BoundingBox& b) {
    const std::unique_ptr<BoundingBox> c(BoundingBox::make_from_area(a.north, a.west, a.south, a.east));
    const std::unique_ptr<BoundingBox> d(BoundingBox::make_from_area(b.north, b.west, b.south, b.east));

    return is_approximately_equal(c->north, d->north) && is_approximately_equal(c->south, d->south)
           && is_approximately_equal(c->west, d->west) && is_approximately_equal(c->east, d->east);
}


}  // namespace eckit::geo::area
