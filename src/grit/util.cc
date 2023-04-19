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


#include "grit/util.h"


namespace grit::util {


template <>
pl_type pl_convert(const pl_type& pl) {
    return pl;
}


double normalise_longitude_to_minimum(double lon, double minimum) {
    while (lon < minimum) {
        lon += 360.;
    }
    while (lon >= minimum + 360.) {
        lon -= 360.;
    }
    return lon;
}


double normalise_longitude_to_maximum(double lon, double maximum) {
    while (lon > maximum) {
        lon -= 360.;
    }
    while (lon <= maximum - 360.) {
        lon += 360.;
    }
    return lon;
}


void normalise_latitude_and_longitude(double& lat, double& lon, double lon_minimum) {
    lat = normalise_longitude_to_minimum(lat, -90.);

    if (lat > 90.) {
        lat -= 180.;
        lon += 180.;
    }

    lon = normalise_longitude_to_minimum(lon, lon_minimum);
}


}  // namespace grit::util
