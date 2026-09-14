// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/figure/OblateSpheroid.h"


namespace eckit::geo::figure {


/// Oblate spheroid parametrised with a geodetic datum
template <class DATUM>
class OblateSpheroidT : public Figure {
public:

    // -- Constructors

    OblateSpheroidT() = default;

    // -- Overridden methods

    double R() const override { return OblateSpheroid::R(DATUM::a, DATUM::b); }
    double a() const override { return DATUM::a; }
    double b() const override { return DATUM::b; }

    /// Surface area [L^2]
    double area() const override { return OblateSpheroid::_area(DATUM::a, DATUM::b); }

    /// Surface area between parallels and meridians [L^2]
    double area(const area::BoundingBox& bbox) const override {
        return OblateSpheroid::_area(DATUM::a, DATUM::b, bbox);
    }
};


}  // namespace eckit::geo::figure
