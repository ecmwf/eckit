// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/figure/SphereT.h"


namespace eckit::geo::figure {


struct DatumSun {
    static constexpr double radius = 695990000.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = false;
};


using Sun = SphereT<DatumSun>;


extern const Sun SUN;


}  // namespace eckit::geo::figure
