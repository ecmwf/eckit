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


#include "eckit/geo/projection/LambertConformalConic.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


LambertConformalConic::LambertConformalConic(const Spec&) {
    // if (fabs(Latin1InDegrees + Latin2InDegrees) < EPSILON) {
    //     grib_context_log(h->context, GRIB_LOG_ERROR,
    //                      "%s: Cannot have equal latitudes for standard parallels on opposite sides of equator",
    //                      ITER);
    //     return GRIB_WRONG_GRID;
    // }
}


Point LambertConformalConic::fwd(const Point&) const {
    NOTIMP;
}


Point LambertConformalConic::inv(const Point&) const {
    NOTIMP;
}


Spec* LambertConformalConic::spec() const {
    NOTIMP;
}


namespace {


double adjust_lon_radians(double lon) {
    while (lon > M_PI) {
        lon -= 2 * M_PI;
    }
    while (lon < -M_PI) {
        lon += 2 * M_PI;
    }
    return lon;
    ß
};


void init_sphere() {
    size_t nv;
    long nx;
    long ny;
    double LoVInDegrees;
    double Dx;
    double Dy;
    double radius            = 1.;
    double latFirstInRadians = 0.;
    double lonFirstInRadians = 0.;
    double LoVInRadians      = 0.;
    double Latin1InRadians   = 0.;
    double Latin2InRadians   = 0.;
    double LaDInRadians      = 0.;

    double n;

    if (fabs(Latin1InRadians - Latin2InRadians) < 1E-09) {
        n = sin(Latin1InRadians);
    }
    else {
        n = log(cos(Latin1InRadians) / cos(Latin2InRadians))
            / log(tan(M_PI_4 + Latin2InRadians / 2.0) / tan(M_PI_4 + Latin1InRadians / 2.0));
    }

    double f         = (cos(Latin1InRadians) * pow(tan(M_PI_4 + Latin1InRadians / 2.0), n)) / n;
    double rho       = radius * f * pow(tan(M_PI_4 + latFirstInRadians / 2.0), -n);
    double rho0_bare = f * pow(tan(M_PI_4 + LaDInRadians / 2.0), -n);
    double rho0      = radius * rho0_bare;  // scaled
    double lonDiff   = adjust_lon_radians(lonFirstInRadians - LoVInRadians);

    double angle = n * lonDiff;
    double x0    = rho * sin(angle);
    double y0    = rho0 - rho * cos(angle);

    double latDeg = 0;
    double lonDeg = 0;

    Point2 p;


    ASSERT(radius > 0);
    ASSERT(n != 0.0);

    double x = p.X / radius;
    double y = rho0_bare - p.Y / radius;

    if (double rho = hypot(x, y); rho != 0.0) {
        if (n < 0.0) {
            rho = -rho;
            x   = -x;
            y   = -y;
        }
        double latRadians = 2. * atan(pow(f / rho, 1.0 / n)) - M_PI_2;
        double lonRadians = atan2(x, y) / n + LoVInRadians;

        auto q = PointLonLat::make(lonRadians * util::RADIAN_TO_DEGREE, latRadians * util::RADIAN_TO_DEGREE);
    }
    else {
        auto q = PointLonLat::make(0.0, n > 0 ? PointLonLat::NORTH_POLE : PointLonLat::SOUTH_POLE);
    }
}


void init_oblate() {
    static constexpr double EPSILON = 1e-10;

    double LoVInDegrees           = 0.;
    double LoVInRadians           = 0.;
    double LaDInRadians           = 0.;
    double LaDInDegrees           = 0.;
    double latFirstInRadians      = 0.;
    double lonFirstInRadians      = 0.;
    double Latin1InRadians        = 0.;
    double Latin2InRadians        = 0.;
    double earthMajorAxisInMetres = 1.;
    double earthMinorAxisInMetres = 1.;

    double x0;
    double y0;

    double sinphi;
    double ts;
    double rh1;

    double ns;  // ratio of angle between meridian
    double F;   // flattening of ellipsoid
    double rh;  // height above ellipsoid

    double sin_po;  // temporary sin value
    double cos_po;  // temporary cos value
    double con;     // temporary variable

    double ms1;  // small m 1
    double ms2;  // small m 2
    double ts0;  // small t 0
    double ts1;  // small t 1
    double ts2;  // small t 2

    auto calculate_eccentricity = [](double minor, double major) {
        const double temp = minor / major;
        return sqrt(1.0 - temp * temp);
    };

    // Compute the constant small m which is the radius of
    // a parallel of latitude, phi, divided by the semimajor axis
    auto compute_m = [](double eccent, double sinphi, double cosphi) {
        const double con = eccent * sinphi;
        return ((cosphi / (sqrt(1.0 - con * con))));
    };

    // Compute the constant small t for use in the forward computations
    auto compute_t = [](double eccent,  // Eccentricity of the spheroid
                        double phi,     // Latitude phi
                        double sinphi)  // Sine of the latitude
    {
        double con = eccent * sinphi;
        con        = pow(((1.0 - con) / (1.0 + con)), 0.5 * eccent);
        return (tan(0.5 * (M_PI_2 - phi)) / con);
    };

    // Function to compute the latitude angle, phi2, for the inverse
    // From the book "Map Projections-A Working Manual-John P. Snyder (1987)"
    // Equation (7-9) involves rapidly converging iteration: Calculate t from (15-11)
    // Then, assuming an initial trial phi equal to (pi/2 - 2*arctan t) in the right side of equation (7-9),
    // calculate phi on the left side. Substitute the calculated phi) into the right side,
    // calculate a new phi, etc., until phi does not change significantly from the preceding trial value of phi
    auto compute_phi = [](double eccent,  // Spheroid eccentricity
                          double ts       // Constant value t
                          ) -> double {
        double eccnth;
        double phi;
        double con;
        double dphi;
        double sinpi;

        eccnth = 0.5 * eccent;
        phi    = M_PI_2 - 2 * atan(ts);

        constexpr int MAX_ITER = 15;
        for (int i = 0; i <= MAX_ITER; i++) {
            sinpi = sin(phi);
            con   = eccent * sinpi;
            dphi  = M_PI_2 - 2 * atan(ts * (pow(((1.0 - con) / (1.0 + con)), eccnth))) - phi;
            phi += dphi;
            if (fabs(dphi) <= 0.0000000001)
                return (phi);
        }

        throw eckit::SeriousBug("Failed to compute the latitude angle, phi2, for the inverse", Here());
    };

    double e = calculate_eccentricity(earthMinorAxisInMetres, earthMajorAxisInMetres);

    sin_po = sin(Latin1InRadians);
    cos_po = cos(Latin1InRadians);
    con    = sin_po;
    ms1    = compute_m(e, sin_po, cos_po);
    ts1    = compute_t(e, Latin1InRadians, sin_po);

    sin_po = sin(Latin2InRadians);
    cos_po = cos(Latin2InRadians);
    ms2    = compute_m(e, sin_po, cos_po);
    ts2    = compute_t(e, Latin2InRadians, sin_po);

    sin_po = sin(LaDInRadians);
    ts0    = compute_t(e, LaDInRadians, sin_po);

    ns = fabs(Latin1InRadians - Latin2InRadians) > EPSILON ? log(ms1 / ms2) / log(ts1 / ts2) : con;
    F  = ms1 / (ns * pow(ts1, ns));
    rh = earthMajorAxisInMetres * F * pow(ts0, ns);

    // Forward projection: convert lat,lon to x,y
    if (fabs(fabs(latFirstInRadians) - M_PI_2) > EPSILON) {
        sinphi = sin(latFirstInRadians);
        ts     = compute_t(e, latFirstInRadians, sinphi);
        rh1    = earthMajorAxisInMetres * F * pow(ts, ns);
    }
    else {
        if (latFirstInRadians * ns <= 0) {
            throw;  // GRIB_GEOCALCULUS_PROBLEM Point cannot be projected: {latFirstInRadians};
        }
        rh1 = 0;
    }

    double theta0 = ns * adjust_lon_radians(lonFirstInRadians - LoVInRadians);
    x0            = -rh1 * sin(theta0);
    y0            = rh1 * cos(theta0) - rh;


    // Inverse projection to convert from x,y to lat,lon
    double x = 0.;
    double y = 0.;

    Point2 p{x - x0, rh - y + y0};

    rh1 = sqrt(p.X * p.X + p.Y * p.Y);
    con = 1.0;
    if (ns <= 0) {
        rh1 = -rh1;
        con = -con;
    }

    double theta = rh1 != 0 ? atan2((con * p.X), (con * p.Y)) : 0.0;

    double latRad
        = rh1 != 0 || ns > 0.0 ? compute_phi(e, pow((rh1 / (earthMajorAxisInMetres * F)), 1.0 / ns)) : -M_PI_2;
    double lonRad = adjust_lon_radians(theta / ns + LoVInRadians);

    auto q = PointLonLat::make(lonRad * util::RADIAN_TO_DEGREE, latRad * util::RADIAN_TO_DEGREE);
}


}  // namespace


}  // namespace eckit::geo::projection
