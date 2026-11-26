// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"


namespace eckit::geo::figure {


class Sphere : public Figure {
public:

    // -- Constructors

    explicit Sphere(double R);
    explicit Sphere(const Spec&);

    // -- Overridden methods

    double R() const override { return R_; }
    double a() const override { return R_; }
    double b() const override { return R_; }

    // -- Class methods

    /// Great-circle central angle between two points [radian]
    static double centralAngle(const PointLonLat&, const PointLonLat&);

    /// Great-circle central angle between two points (Cartesian coordinates)
    static double centralAngle(double radius, const PointXYZ&, const PointXYZ&);

    /// Great-circle distance between two points
    static double distance(double radius, const PointLonLat&, const PointLonLat&);

    /// Great-circle distance between two points (Cartesian coordinates)
    static double distance(double radius, const PointXYZ&, const PointXYZ&);

    /// Great-circle intermediate latitude provided two circle points and intermediate longitude [degree]
    static double greatCircleLatitudeGivenLongitude(const PointLonLat&, const PointLonLat&, double lon);

    /// Great-circle intermediate longitude(s) provided two circle points and intermediate latitude [degree]
    static void greatCircleLongitudeGivenLatitude(const PointLonLat&, const PointLonLat&, double lat, double& lon1,
                                                  double& lon2);

    /// Convert spherical to Cartesian coordinates
    static PointXYZ convertSphericalToCartesian(double radius, const PointLonLat&, double height = 0.);

    /// Convert Cartesian to spherical coordinates
    static PointLonLat convertCartesianToSpherical(double radius, const PointXYZ&);

private:

    // -- Members

    const double R_;
};


}  // namespace eckit::geo::figure
