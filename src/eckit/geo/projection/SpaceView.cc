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


#include "eckit/geo/projection/SpaceView.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


SpaceView::SpaceView(const Spec&) {
    // Orthographic not supported. This happens when Nr (camera altitude) is missing

    // if (latOfSubSatellitePointInDegrees != 0.0) {
    //     grib_context_log(h->context, GRIB_LOG_ERROR,
    //                      "%s: Key %s must be 0 (satellite must be located in the equator plane)", ITER,
    //                      sLatOfSubSatellitePointInDegrees);
    //     return GRIB_GEOCALCULUS_PROBLEM;
    // }

    // (orientationInDegrees != 0.0) not spported
}


Point2 SpaceView::fwd(const PointLonLat&) const {
    NOTIMP;
}


PointLonLat SpaceView::inv(const Point2&) const {
    NOTIMP;
}


void SpaceView::fill_spec(spec::Custom& custom) const {
    ProjectionOnFigure::fill_spec(custom);

    custom.set("projection", "geos");  //?
    NOTIMP;
}


namespace {


void init() {
    double lonOfSubSatellitePointInDegrees = 0.;
    double nrInRadiusOfEarth               = 1.;
    double xpInGridLengths                 = 0.;
    double ypInGridLengths                 = 0.;
    long Xo                                = 0;
    long Yo                                = 0;

    double earthMajorAxis = 1. / 1000.;  // [km]
    double earthMinorAxis = 1. / 1000.;

    double angular_size = 2.0 * asin(1.0 / nrInRadiusOfEarth);
    double height       = nrInRadiusOfEarth * earthMajorAxis;

    double lop = lonOfSubSatellitePointInDegrees;

    double dx = 1.;
    double dy = 1.;

    auto x0 = static_cast<double>(Xo);
    auto y0 = static_cast<double>(Yo);

    double xp = xpInGridLengths - x0;
    double yp = ypInGridLengths - y0;

    double rx = angular_size / dx;
    double ry = (earthMinorAxis / earthMajorAxis) * angular_size / dy;

    Point2 p;
    Point2 q{(p.X - xp) * rx, (p.Y - yp) * ry};

    double factor_1 = height * height - earthMajorAxis * earthMajorAxis;
    double factor_2 = (earthMajorAxis / earthMinorAxis) * (earthMajorAxis / earthMinorAxis);
    double factor_3 = (1 + (factor_2 - 1.0) * sin(q.Y) * sin(q.Y));

    double Sd = height * cos(q.X) * cos(q.Y);
    Sd        = sqrt(Sd * Sd - factor_3 * factor_1);
    if (Sd <= 0.0) {
        Point q;
    }
    else {
        double Sn  = (height * cos(q.X) * cos(q.Y) - Sd) / factor_3;
        double S1  = height - Sn * cos(q.X) * cos(q.Y);
        double S2  = Sn * sin(q.X) * cos(q.Y);
        double S3  = Sn * sin(q.Y);
        double Sxy = sqrt(S1 * S1 + S2 * S2);

        auto lonr = atan(S2 / S1) + lop;
        auto latr = atan(factor_2 * S3 / Sxy);
        PointLonLat::make(util::RADIAN_TO_DEGREE * lonr, util::RADIAN_TO_DEGREE * latr);
    }
}


}  // namespace


}  // namespace eckit::geo::projection
