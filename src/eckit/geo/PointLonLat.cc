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

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


PointLonLat::PointLonLat(double lon, double lat) :
    P{lon, lat} {
    if (!(-90. <= lat && lat <= 90.)) {
        throw BadValue("PointLonLat: invalid latitude");
    }
}


double PointLonLat::normalise_angle_to_minimum(double a, double minimum) {
    while (a < minimum) {
        a += 360.;
    }
    while (a >= minimum + 360.) {
        a -= 360.;
    }
    return a;
}


double PointLonLat::normalise_angle_to_maximum(double a, double maximum) {
    while (a > maximum) {
        a -= 360.;
    }
    while (a <= maximum - 360.) {
        a += 360.;
    }
    return a;
}


bool points_equal(const PointLonLat& a, const PointLonLat& b, double eps) {
    // FIXME
    // solved {180., 0.} == {-180., 0.}
    // could be more performant
    auto c = PointLonLat::make(a.lon, a.lat);
    auto d = PointLonLat::make(b.lon, b.lat);
    return types::is_approximately_equal(c.lon, d.lon, eps) && types::is_approximately_equal(c.lat, d.lat, eps);
}


}  // namespace eckit::geo
