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


#include "eckit/geo/range/RegularLatitude.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


RegularLatitude::RegularLatitude(double _inc, double _a, double _b, double _ref) :
    Regular(_inc, _a, _b, _ref, PointLonLat::EPS) {}


RegularLatitude::RegularLatitude(size_t n, double _a, double _b) : Regular(n, _a, _b, false, PointLonLat::EPS) {
    ASSERT(SOUTH_POLE.lat <= a() && a() <= NORTH_POLE.lat);
    ASSERT(SOUTH_POLE.lat <= b() && b() <= NORTH_POLE.lat);
}


Range* RegularLatitude::make_range_cropped(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b) ||
           (types::is_approximately_equal(a(), b(), PointLonLat::EPS) &&
            types::is_approximately_equal(crop_a, crop_b, PointLonLat::EPS)));

    if (types::is_approximately_equal(crop_a, crop_b, PointLonLat::EPS)) {
        NOTIMP;  // FIXME
    }

    if (a() < b()) {
        ASSERT(a() <= crop_a && crop_b <= b());  // FIXME do better

        const auto inc(increment());
        const auto d  = (a() / inc).decimalPart() * inc;
        const auto _a = adjust(crop_a - d, inc, true) + d;
        const auto _b = adjust(crop_b - d, inc, false) + d;

        const auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        const auto n = static_cast<size_t>(nf.integralPart() + 1);
        ASSERT(0 < n && n <= size());

        return new RegularLatitude(n, _a, _b);
    }

    NOTIMP;
}


}  // namespace eckit::geo::range
