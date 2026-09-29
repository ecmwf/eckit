// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef EllipsoidOfRevolution_H
#define EllipsoidOfRevolution_H

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

class Point2;
class Point3;

//----------------------------------------------------------------------------------------------------------------------

struct EllipsoidOfRevolution {
    // Convert elliptic coordinates to Cartesian
    static void convertSphericalToCartesian(const double& radiusA, const double& radiusB, const Point2& Alonlat,
                                            Point3& B, double height = 0., bool normalise_angle = false);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
