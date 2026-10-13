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

    bool is_default() const override { return DATUM::default_figure; }
};


}  // namespace eckit::geo::figure
