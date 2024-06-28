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


#include "eckit/geo/PointLonLat.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointLonLat::value_type PointLonLat::normalise_angle_to_minimum(value_type a, value_type minimum) {
    const auto modulus = [](auto a) { return a - FULL_ANGLE * std::floor(a / FULL_ANGLE); };

    auto diff = a - minimum;
    return 0. <= diff && diff < FULL_ANGLE ? a : (modulus(diff) + minimum);
}


PointLonLat::value_type PointLonLat::normalise_angle_to_maximum(value_type a, value_type maximum) {
    const auto modulus = [](auto a) { return a - FULL_ANGLE * std::ceil(a / FULL_ANGLE); };

    auto diff = a - maximum;
    return -FULL_ANGLE < diff && diff <= 0. ? a : (modulus(diff) + maximum);
}


void PointLonLat::assert_latitude_range(const PointLonLat& P) {
    if (!(-RIGHT_ANGLE <= P.lat && P.lat <= RIGHT_ANGLE)) {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<value_type>::max_digits10);
        oss << "Invalid latitude [degree] " << P.lat;
        throw BadValue(oss.str(), Here());
    }
}


PointLonLat PointLonLat::make(value_type lon, value_type lat, value_type lon_minimum, value_type eps) {
    lat = normalise_angle_to_minimum(lat, -RIGHT_ANGLE);

    if (types::is_strictly_greater(lat, RIGHT_ANGLE, eps)) {
        lat = FLAT_ANGLE - lat;
        lon += FLAT_ANGLE;
    }

    return types::is_approximately_equal(lat, RIGHT_ANGLE, eps) ? NORTH_POLE
           : types::is_approximately_equal(lat, -RIGHT_ANGLE, eps)
               ? SOUTH_POLE
               : PointLonLat{normalise_angle_to_minimum(lon, lon_minimum), lat};
}


PointLonLat PointLonLat::make_from_lonlatr(value_type lonr, value_type latr, value_type lonr_minimum) {
    return make(util::RADIAN_TO_DEGREE * lonr, util::RADIAN_TO_DEGREE * latr, util::RADIAN_TO_DEGREE * lonr_minimum);
}


PointLonLat PointLonLat::componentsMin(const PointLonLat& p, const PointLonLat& q) {
    return {std::min(p.lon, q.lon), std::min(p.lat, q.lat)};
}


PointLonLat PointLonLat::componentsMax(const PointLonLat& p, const PointLonLat& q) {
    return {std::max(p.lon, q.lon), std::max(p.lat, q.lat)};
}


bool points_equal(const PointLonLat& a, const PointLonLat& b, PointLonLat::value_type eps) {
    const auto c = PointLonLat::make(a.lon, a.lat, 0., eps);
    const auto d = PointLonLat::make(b.lon, b.lat, 0., eps);
    return types::is_approximately_equal(c.lon, d.lon, eps) && types::is_approximately_equal(c.lat, d.lat, eps);
}


const PointLonLat NORTH_POLE{0., PointLonLat::RIGHT_ANGLE};
const PointLonLat SOUTH_POLE{0., -PointLonLat::RIGHT_ANGLE};


}  // namespace eckit::geo
