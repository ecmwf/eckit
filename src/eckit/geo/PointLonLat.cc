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

#include <cmath>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointLonLat::value_type PointLonLat::normalise_angle_to_minimum(value_type a, value_type minimum) {
    static const auto modulo_globe = [](auto a) { return a - GLOBE * std::floor(a / GLOBE); };

    auto diff = a - minimum;
    return 0. <= diff && diff < GLOBE ? a : (modulo_globe(diff) + minimum);
}


PointLonLat::value_type PointLonLat::normalise_angle_to_maximum(value_type a, value_type maximum) {
    auto modulo_globe = [](auto a) { return a - GLOBE * std::ceil(a / GLOBE); };

    auto diff = a - maximum;
    return -GLOBE < diff && diff <= 0. ? a : (modulo_globe(diff) + maximum);
}


void PointLonLat::assert_latitude_range(const PointLonLat& P) {
    if (!(SOUTH_POLE <= P.lat && P.lat <= NORTH_POLE)) {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<value_type>::max_digits10);
        oss << "Invalid latitude [degree] " << P.lat;
        throw BadValue(oss.str(), Here());
    }
}


PointLonLat PointLonLat::make(value_type lon, value_type lat, value_type lon_minimum, value_type eps) {
    lat = normalise_angle_to_minimum(lat, SOUTH_POLE);

    if (types::is_strictly_greater(lat, NORTH_POLE, eps)) {
        lat = GLOBE / 2. - lat;
        lon += GLOBE / 2.;
    }

    return types::is_approximately_equal(lat, NORTH_POLE, eps) ? PointLonLat{0., NORTH_POLE}
           : types::is_approximately_equal(lat, SOUTH_POLE, eps)
               ? PointLonLat{EQUATOR, SOUTH_POLE}
               : PointLonLat{normalise_angle_to_minimum(lon, lon_minimum), lat};
}


bool points_equal(const PointLonLat& a, const PointLonLat& b, PointLonLat::value_type eps) {
    const auto c = PointLonLat::make(a.lon, a.lat, PointLonLat::EQUATOR, eps);
    const auto d = PointLonLat::make(b.lon, b.lat, PointLonLat::EQUATOR, eps);
    return types::is_approximately_equal(c.lon, d.lon, eps) && types::is_approximately_equal(c.lat, d.lat, eps);
}


}  // namespace eckit::geo
