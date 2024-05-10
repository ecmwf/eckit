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


#include "eckit/geo/projection/LambertAzimuthalEqualArea.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Spec&) {}


Point LambertAzimuthalEqualArea::fwd(const Point&) const {
    NOTIMP;
}


Point LambertAzimuthalEqualArea::inv(const Point&) const {
    NOTIMP;
}


Spec* LambertAzimuthalEqualArea::spec() const {
    NOTIMP;
}


static constexpr double EPSILON = 1e-10;

static constexpr double P00 = .33333333333333333333;  //   1 /     3
static constexpr double P01 = .17222222222222222222;  //  31 /   180
static constexpr double P02 = .10257936507936507937;  // 517 /  5040
static constexpr double P10 = .06388888888888888888;  //  23 /   360
static constexpr double P11 = .06640211640211640212;  // 251 /  3780
static constexpr double P20 = .01677689594356261023;  // 761 / 45360


static double pj_authlat(double beta, const double* APA) {
    double t = beta + beta;
    return (beta + APA[0] * sin(t) + APA[1] * sin(t + t) + APA[2] * sin(t + t + t));
}


static double pj_qsfn(double sinphi, double e, double one_es) {
    if (e < 1.e-7) {
        return sinphi + sinphi;
    }

    double con  = e * sinphi;
    double div1 = 1.0 - con * con;
    double div2 = 1.0 + con;

    /* avoid zero division, fail gracefully */
    return div1 == 0.0 || div2 == 0.0 ? HUGE_VAL : (one_es * (sinphi / div1 - (.5 / e) * log((1. - con) / div2)));
}


static void init_oblate() {
    double earthMinorAxisInMetres    = 0;
    double earthMajorAxisInMetres    = 0;
    double latFirstInRadians         = 0;
    double lonFirstInRadians         = 0;
    double centralLongitudeInRadians = 0;
    double standardParallelInRadians = 0;

    const double temp   = (earthMajorAxisInMetres - earthMinorAxisInMetres) / earthMajorAxisInMetres;
    const double es     = 2 * temp - temp * temp;
    const double one_es = 1.0 - es;
    const double e      = sqrt(es);

    const double coslam = cos(lonFirstInRadians - centralLongitudeInRadians); /* cos(lp.lam) */
    const double sinlam = sin(lonFirstInRadians - centralLongitudeInRadians);
    const double sinphi = sin(latFirstInRadians); /* sin(lp.phi) */
    const double q      = pj_qsfn(sinphi, e, one_es);

    const double t = fabs(standardParallelInRadians);
    if (t > M_PI_2 + EPSILON) {
        throw;  // GRIB_GEOCALCULUS_PROBLEM;
    }

    const double Q__qp = pj_qsfn(1.0, e, one_es);

    const double APA[3]
        = {es * P00 + es * es * P01 + es * es * es * P02, es * es * P10 + es * es * es * P11, es * es * es * P20};

    const double Q__rq    = sqrt(0.5 * Q__qp);
    const double sinphi_  = sin(standardParallelInRadians); /*  (P->phi0); */
    const double Q__sinb1 = pj_qsfn(sinphi_, e, one_es) / Q__qp;
    const double Q__cosb1 = sqrt(1.0 - Q__sinb1 * Q__sinb1);
    const double Q__dd
        = Q__cosb1 == 0 ? 1. : cos(standardParallelInRadians) / (sqrt(1. - es * sinphi_ * sinphi_) * Q__rq * Q__cosb1);
    const double Q__xmf = Q__rq * Q__dd;
    const double Q__ymf = Q__rq / Q__dd;

    const double sinb  = q / Q__qp;
    const double cosb2 = 1.0 - sinb * sinb;
    const double cosb  = cosb2 > 0 ? sqrt(cosb2) : 0;

    double b = 1. + Q__sinb1 * sinb + Q__cosb1 * cosb * coslam;
    if (fabs(b) < EPSILON) {
        throw;  // GRIB_GEOCALCULUS_PROBLEM;
    }
    b = sqrt(2.0 / b);

    /* OBLIQUE */
    const double y0 = Q__ymf * b * (Q__cosb1 * sinb - Q__sinb1 * cosb * coslam);
    const double x0 = Q__xmf * b * cosb * sinlam;

    double x = x0;
    double y = y0;

    double xy_x = x / Q__dd;
    double xy_y = y * Q__dd;
    double rho  = hypot(xy_x, xy_y);
    ASSERT(rho >= EPSILON);

    const double asin_arg = (0.5 * rho / Q__rq);
    if (asin_arg < -1.0 || asin_arg > 1.0) {
        // Invalid value: arcsin argument={asin_arg}
        throw;  // GRIB_GEOCALCULUS_PROBLEM;
    }

    double cCe = cos(2. * asin(asin_arg));
    double sCe = sin(2. * asin(asin_arg));
    xy_x *= sCe;

    double ab = cCe * Q__sinb1 + xy_y * sCe * Q__cosb1 / rho;
    xy_y      = rho * Q__cosb1 * cCe - xy_y * Q__sinb1 * sCe;

    double latr = pj_authlat(asin(ab), APA);
    double lonr = atan2(xy_x, xy_y) + centralLongitudeInRadians;

    PointLonLat::make(lonr * util::RADIAN_TO_DEGREE, latr * util::RADIAN_TO_DEGREE);
}


void init_sphere() {
    double latFirstInRadians = 0.;
    double lonFirstInRadians = 0.;
    double radius            = 1.;

    double lambda0 = 0.;  // centralLongitudeInRadians;
    double phi1    = 0.;  // standardParallelInRadians;

    /* compute xFirst,yFirst in metres */
    auto cosphi1    = cos(phi1);
    auto sinphi1    = sin(phi1);
    auto sinphi     = sin(latFirstInRadians);
    auto cosphi     = cos(latFirstInRadians);
    auto cosdlambda = cos(lonFirstInRadians - lambda0);
    auto sindlambda = sin(lonFirstInRadians - lambda0);

    auto kp     = radius * sqrt(2.0 / (1 + sinphi1 * sinphi + cosphi1 * cosphi * cosdlambda));
    auto xFirst = kp * cosphi * sindlambda;
    auto yFirst = kp * (cosphi1 * sinphi - sinphi1 * cosphi * cosdlambda);

    Point2 p;

    double lonr = phi1;
    double latr = lambda0;

    if (auto x = p.X, y = p.Y, rho = sqrt(x * x + y * y); rho > EPSILON) {
        double c    = 2 * asin(rho / (2.0 * radius));
        double cosc = cos(c);
        double sinc = sin(c);
        latr        = asin(cosc * sinphi1 + y * sinc * cosphi1 / rho);
        lonr        = (lambda0 + atan2(x * sinc, rho * cosphi1 * cosc - y * sinphi1 * sinc));
    }

    PointLonLat::make(lonr * util::RADIAN_TO_DEGREE, latr * util::RADIAN_TO_DEGREE);
}


}  // namespace eckit::geo::projection
