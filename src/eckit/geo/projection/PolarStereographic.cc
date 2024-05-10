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


#include "eckit/geo/projection/PolarStereographic.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


PolarStereographic::PolarStereographic(const Spec&) {}


Point PolarStereographic::fwd(const Point&) const {
    NOTIMP;
}


Point PolarStereographic::inv(const Point&) const {
    NOTIMP;
}


Spec* PolarStereographic::spec() const {
    NOTIMP;
}


namespace {


void init() {
    static constexpr double EPSILON = 1e-10;

    const double radius           = 1.;
    const double centralLongitude = util::DEGREE_TO_RADIAN * 0. /*centralLongitudeInDegrees*/;
    const double centralLatitude  = util::DEGREE_TO_RADIAN * 0. /*centralLatitudeInDegrees*/;
    const double lonFirst         = util::DEGREE_TO_RADIAN * 0. /*lonFirstInDegrees*/;
    const double latFirst         = util::DEGREE_TO_RADIAN * 0. /*latFirstInDegrees*/;

    const double sign = centralLatitude < 0 ? -1. : 1.;
    const bool ind    = fabs(fabs(centralLatitude) - M_PI_2) > EPSILON;

    const double mcs = ind ? cos(sign * centralLatitude) : 0.;
    const double tcs = ind ? tan(0.5 * (M_PI_2 - sign * centralLatitude)) : 0.;

    /* Forward projection from initial lat,lon to initial x,y */
    double tsf    = tan(0.5 * (M_PI_2 - sign * latFirst));
    double height = ind ? radius * mcs * tsf / tcs : 2.0 * radius * tsf;

    double a  = sign * (lonFirst - centralLongitude);
    double x0 = -sign * height * sin(a);
    double y0 = sign * height * cos(a);

    /* Inverse projection from x,y to lat,lon */
    double x = 0;
    double y = 0;
    Point2 p{(x - x0) * sign, (y - y0) * sign};

    double rh  = sqrt(p.X * p.X + p.Y * p.Y);
    double tsi = ind ? rh * tcs / (radius * mcs) : rh / (radius * 2.0);

    double latr = sign * (M_PI_2 - 2 * atan(tsi));
    double lonr = rh == 0 ? sign * centralLongitude : sign * atan2(p.X, -p.Y) + centralLongitude;

    PointLonLat::make(lonr * util::RADIAN_TO_DEGREE, latr * util::RADIAN_TO_DEGREE);
}


}  // namespace


}  // namespace eckit::geo::projection
