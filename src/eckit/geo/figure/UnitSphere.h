// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/figure/SphereT.h"


namespace eckit::geo::figure {


struct DatumUnit {
    static constexpr double radius = 1.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = false;
};


using UnitSphere = SphereT<DatumUnit>;


}  // namespace eckit::geo::figure
