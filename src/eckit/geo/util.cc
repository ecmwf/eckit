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


#include "eckit/geo/util.h"


namespace eckit::geo::util {


template <>
pl_type pl_convert(const pl_type& pl) {
    return pl;
}


double normalise_angle_to_minimum(double a, double minimum) {
    while (a < minimum) {
        a += 360.;
    }
    while (a >= minimum + 360.) {
        a -= 360.;
    }
    return a;
}


double normalise_angle_to_maximum(double a, double maximum) {
    while (a > maximum) {
        a -= 360.;
    }
    while (a <= maximum - 360.) {
        a += 360.;
    }
    return a;
}


}  // namespace eckit::geo::util
