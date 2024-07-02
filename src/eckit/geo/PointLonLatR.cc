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


#include "eckit/geo/PointLonLatR.h"

#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointLonLatR::value_type PointLonLatR::normalise_angle_to_minimum(value_type a, value_type minimum) {
    static const auto modulus = [](auto a) { return a - FULL_ANGLE * std::floor(a / FULL_ANGLE); };

    auto diff = a - minimum;
    return 0. <= diff && diff < FULL_ANGLE ? a : (modulus(diff) + minimum);
}


PointLonLatR::value_type PointLonLatR::normalise_angle_to_maximum(value_type a, value_type maximum) {
    auto modulus = [](auto a) { return a - FULL_ANGLE * std::ceil(a / FULL_ANGLE); };

    auto diff = a - maximum;
    return -FULL_ANGLE < diff && diff <= 0. ? a : (modulus(diff) + maximum);
}


PointLonLatR PointLonLatR::make(value_type lonr, value_type latr, value_type lonr_minimum, value_type eps) {
    latr = normalise_angle_to_minimum(latr, -RIGHT_ANGLE);

    if (types::is_strictly_greater(latr, RIGHT_ANGLE, eps)) {
        latr = FLAT_ANGLE - latr;
        lonr += FLAT_ANGLE;
    }

    return types::is_approximately_equal(latr, RIGHT_ANGLE, eps) ? NORTH_POLE_R
           : types::is_approximately_equal(latr, -RIGHT_ANGLE, eps)
               ? SOUTH_POLE_R
               : PointLonLatR{normalise_angle_to_minimum(lonr, lonr_minimum), latr};
}


PointLonLatR PointLonLatR::make_from_lonlat(value_type lon, value_type lat, value_type lon_minimum) {
    return make(util::DEGREE_TO_RADIAN * lon, util::DEGREE_TO_RADIAN * lat, util::DEGREE_TO_RADIAN * lon_minimum);
}


bool points_equal(const PointLonLatR& a, const PointLonLatR& b, PointLonLatR::value_type eps) {
    const auto c = PointLonLatR::make(a.lonr, a.latr, 0., eps);
    const auto d = PointLonLatR::make(b.lonr, b.latr, 0., eps);
    return types::is_approximately_equal(c.lonr, d.lonr, eps) && types::is_approximately_equal(c.latr, d.latr, eps);
}


const PointLonLatR NORTH_POLE_R{0., PointLonLatR::RIGHT_ANGLE};
const PointLonLatR SOUTH_POLE_R{0., -PointLonLatR::RIGHT_ANGLE};


}  // namespace eckit::geo
