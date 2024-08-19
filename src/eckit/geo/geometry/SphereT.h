/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/Point3.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/geometry/Sphere.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo::geometry {


/// Sphere parametrised with a geodetic datum
template <class DATUM>
struct SphereT {

    /// Sphere radius in metres
    inline static double radius() { return DATUM::radius(); }

    /// Great-circle central angle between two points [radian]
    inline static double centralAngle(const PointLonLat& A, const PointLonLat& B) { return Sphere::centralAngle(A, B); }

    /// Great-circle central angle between two points (Cartesian coordinates) in radians
    inline static double centralAngle(const Point3& A, const Point3& B) {
        return Sphere::centralAngle(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points [m]
    inline static double distance(const PointLonLat& A, const PointLonLat& B) {
        return Sphere::distance(DATUM::radius(), A, B);
    }

    /// Great-circle distance between two points (Cartesian coordinates) [m]
    inline static double distance(const Point3& A, const Point3& B) { return Sphere::distance(DATUM::radius(), A, B); }

    /// Surface area [m ** 2]
    inline static double area() { return Sphere::area(DATUM::radius()); }

    /// Surface area between parallels and meridians [m ** 2]
    inline static double area(const area::BoundingBox& bbox) { return Sphere::area(DATUM::radius(), bbox); }

    /// Great-circle intermediate latitude provided two circle points and intermediate longitude [degree]
    inline static double greatCircleLatitudeGivenLongitude(const PointLonLat& A, const PointLonLat& B, double lon) {
        return Sphere::greatCircleLatitudeGivenLongitude(A, B, lon);
    }

    /// Great-circle intermediate longitude(s) provided two circle points and intermediate latitude [degree]
    inline static void greatCircleLongitudeGivenLatitude(const PointLonLat& A, const PointLonLat& B, double lat,
                                                         double& lon1, double& lon2) {
        return Sphere::greatCircleLongitudeGivenLatitude(A, B, lat, lon1, lon2);
    }

    /// Convert spherical to Cartesian coordinates
    inline static Point3 convertSphericalToCartesian(const PointLonLat& P, double height = 0.) {
        return Sphere::convertSphericalToCartesian(DATUM::radius(), P, height);
    }

    /// Convert Cartesian to spherical coordinates
    inline static PointLonLat convertCartesianToSpherical(const Point3& P) {
        return Sphere::convertCartesianToSpherical(DATUM::radius(), P);
    }
};


}  // namespace eckit::geo::geometry
