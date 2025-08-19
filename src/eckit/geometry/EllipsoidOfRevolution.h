/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
