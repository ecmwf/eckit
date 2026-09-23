// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
    double area(const area::BoundingBox& bbox) const override { return _area(a_, b_, bbox); }

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
