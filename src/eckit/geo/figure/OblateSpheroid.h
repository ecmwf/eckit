// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"


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

    // -- Class methods

    /// Radius
    static double R(double a, double b);

    static double eccentricity(double a, double b);
    static double flattening(double a, double b);

    /// Convert geocentric coordinates to Cartesian
    static PointXYZ convertSphericalToCartesian(double a, double b, const PointLonLat&, double height = 0.);

private:

    // -- Members

    const double a_;
    const double b_;
};


}  // namespace eckit::geo::figure
