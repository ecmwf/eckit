// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/figure/OblateSpheroidT.h"
#include "eckit/geo/figure/SphereT.h"


namespace eckit::geo::figure {


struct DatumIFS {
    static constexpr double radius = 6371229.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = true;
};


struct DatumGRIB1 {
    static constexpr double radius = 6367470.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = true;
};


struct DatumWgs84Sphere {
    static constexpr double radius = 6371200.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = false;
};


struct DatumGrs80 {
    static constexpr double a = 6378137.;
    static constexpr double b = 6356752.314140;

    static constexpr bool default_figure = false;
};


struct DatumWgs84 {
    static constexpr double a = 6378137.;
    static constexpr double b = 6356752.314245;

    static constexpr bool default_figure = false;
};


// Note: 1/f = 298.25, other references have 1/297
struct DatumIau1965 {
    static constexpr double a = 6378160.;
    static constexpr double b = 6356775.;

    static constexpr bool default_figure = false;
};


using Earth            = SphereT<DatumIFS>;
using EarthGrib1       = SphereT<DatumGRIB1>;
using EarthGrs80       = OblateSpheroidT<DatumGrs80>;
using EarthIau1965     = OblateSpheroidT<DatumIau1965>;
using EarthWgs84       = OblateSpheroidT<DatumWgs84>;
using EarthWgs84Sphere = SphereT<DatumWgs84Sphere>;


extern const Earth EARTH;
extern const EarthGrib1 EARTH_GRIB1;
extern const EarthGrs80 EARTH_GRS80;
extern const EarthIau1965 EARTH_IAU1965;
extern const EarthWgs84 EARTH_WGS84;
extern const EarthWgs84Sphere EARTH_WGS84_SPHERE;


}  // namespace eckit::geo::figure
