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


namespace eckit::geo {
class Point3;
class PointLonLat;
}  // namespace eckit::geo


namespace eckit::geo::geometry {


struct OblateSpheroid {
    /// Elliptic eccentricity
    static double eccentricity(double a, double b);

    /// Flattening
    static double flattening(double a, double b);

    /// Surface area [m ** 2]
    static double area(double a, double b);

    /// Convert geocentric coordinates to Cartesian
    static Point3 convertSphericalToCartesian(double a, double b, const PointLonLat&, double height = 0.);
};


}  // namespace eckit::geo::geometry
