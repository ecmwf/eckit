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


#pragma once

#include "eckit/geo/Figure.h"


namespace eckit::geo::figure {


class OblateSpheroid : public Figure {
public:

    // -- Constructors

    OblateSpheroid(double a, double b);
    explicit OblateSpheroid(const Spec&);

    // -- Overridden methods

    double R() const override;
    double a() const override { return a_; }
    double b() const override { return b_; }

    /// Surface area [L^2]
    double area() const override { return _area(a_, b_); }

    /// Surface area between parallels and meridians [L^2]
    double area(const area::BoundingBox& bbox) override { return _area(a_, b_, bbox); }

    // -- Class methods

    /// Radius
    static double R(double a, double b);

    /// Elliptic eccentricity
    static double eccentricity(double a, double b);

    /// Flattening
    static double flattening(double a, double b);

    /// Surface area [L^2]
    static double _area(double a, double b);

    /// Surface area between parallels and meridians [L^2]
    static double _area(double a, double b, const area::BoundingBox&);

    /// Convert geocentric coordinates to Cartesian
    static PointXYZ convertSphericalToCartesian(double a, double b, const PointLonLat&, double height = 0.);

private:

    // -- Members

    const double a_;
    const double b_;
};


}  // namespace eckit::geo::figure
