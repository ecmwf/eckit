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
    static const auto modulo_globe = [](auto a) { return a - GLOBE * std::floor(a / GLOBE); };

    auto diff = a - minimum;
    return 0. <= diff && diff < GLOBE ? a : (modulo_globe(diff) + minimum);
}


PointLonLatR::value_type PointLonLatR::normalise_angle_to_maximum(value_type a, value_type maximum) {
    auto modulo_globe = [](auto a) { return a - GLOBE * std::ceil(a / GLOBE); };

    auto diff = a - maximum;
    return -GLOBE < diff && diff <= 0. ? a : (modulo_globe(diff) + maximum);
}


PointLonLatR PointLonLatR::make(value_type lon, value_type lat, value_type lon_minimum, value_type eps) {
    lat = normalise_angle_to_minimum(lat, SOUTH_POLE);

    if (types::is_strictly_greater(lat, NORTH_POLE, eps)) {
        lat = GLOBE / 2. - lat;
        lon += GLOBE / 2.;
    }

    return types::is_approximately_equal(lat, NORTH_POLE, eps) ? PointLonLatR{0., NORTH_POLE}
           : types::is_approximately_equal(lat, SOUTH_POLE, eps)
               ? PointLonLatR{EQUATOR, SOUTH_POLE}
               : PointLonLatR{normalise_angle_to_minimum(lon, lon_minimum), lat};
}


PointLonLatR PointLonLatR::make_from_lonlat(value_type lon, value_type lat) {
    return make(util::DEGREE_TO_RADIAN * lon, util::DEGREE_TO_RADIAN * lat);
}


bool points_equal(const PointLonLatR& a, const PointLonLatR& b, PointLonLatR::value_type eps) {
    const auto c = PointLonLatR::make(a.lonr, a.latr, PointLonLatR::EQUATOR, eps);
    const auto d = PointLonLatR::make(b.lonr, b.latr, PointLonLatR::EQUATOR, eps);
    return types::is_approximately_equal(c.lonr, d.lonr, eps) && types::is_approximately_equal(c.latr, d.latr, eps);
}

}  // namespace eckit::geo
