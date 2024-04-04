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

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointLonLat::PointLonLat(double lon, double lat) :
    container_type{lon, lat} {
    if (!(-90. <= lat && lat <= 90.)) {
        throw BadValue("PointLonLat: invalid latitude");
    }
}


double PointLonLat::normalise_angle_to_minimum(double a, double minimum) {
    auto modulo_360 = [](double a) { return a - 360. * std::floor(a / 360.); };

    auto diff = a - minimum;
    return 0. <= diff && diff < 360. ? a : modulo_360(diff) + minimum;
}


double PointLonLat::normalise_angle_to_maximum(double a, double maximum) {
    auto modulo_360 = [](double a) { return a - 360. * std::ceil(a / 360.); };

    auto diff = a - maximum;
    return -360. < diff && diff <= 0. ? a : modulo_360(a - maximum) + maximum;
}


PointLonLat PointLonLat::make(double lon, double lat, double lon_minimum, double eps) {
    lat = normalise_angle_to_minimum(lat, -90.);

    if (types::is_strictly_greater(lat, 90., eps)) {
        lat = 180. - lat;
        lon += 180.;
    }

    return types::is_approximately_equal(lat, 90., eps) ? PointLonLat{0., 90.}
           : types::is_approximately_equal(lat, -90., eps)
               ? PointLonLat{0., -90.}
               : PointLonLat{normalise_angle_to_minimum(lon, lon_minimum), lat};
}


bool points_equal(const PointLonLat& a, const PointLonLat& b, double eps) {
    auto c = PointLonLat::make(a.lon, a.lat, 0., eps);
    auto d = PointLonLat::make(b.lon, b.lat, 0., eps);
    return types::is_approximately_equal(c.lon, d.lon, eps) && types::is_approximately_equal(c.lat, d.lat, eps);
}


}  // namespace eckit::geo
