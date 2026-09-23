// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/figure/SphereT.h"


namespace eckit::geo::figure {


/// Definition of a unit datum
struct DatumUnit {
    static constexpr double radius = 1.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = false;
};


/// Definition of a unit sphere
using UnitSphere = SphereT<DatumUnit>;


}  // namespace eckit::geo::figure
