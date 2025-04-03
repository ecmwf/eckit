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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXYZ.h"
#include "eckit/geo/figure/OblateSpheroid.h"


namespace eckit::geo::area {
class BoundingBox;
}


namespace eckit::geo::figure {


/// Oblate spheroid parametrised with a geodetic datum
template <class DATUM>
class OblateSpheroidT : public Figure {
public:

    // -- Constructors

    explicit OblateSpheroidT() {}

    // -- Overridden methods

    double R() const override { throw BadValue("geo::figure::OblateSpheroidT::R"); }
    double a() const override { return DATUM::a; }
    double b() const override { return DATUM::b; }

    // -- Class methods

    /// Elliptic eccentricity
    static double eccentricity() { return OblateSpheroid::eccentricity(DATUM::a, DATUM::b); }

    /// Flattening
    static double flattening() { return OblateSpheroid::flattening(DATUM::a, DATUM::b); }

    /// Surface area [m ** 2]
    inline static double area() { return OblateSpheroid::area(DATUM::a, DATUM::b); }

    /// Convert spherical to Cartesian coordinates
    inline static PointXYZ convertSphericalToCartesian(const PointLonLat& P, double height = 0.) {
        return OblateSpheroid::convertSphericalToCartesian(DATUM::a, DATUM::b, P, height);
    }
};


}  // namespace eckit::geo::figure
