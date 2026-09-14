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
};


struct DatumGRIB1 {
    static constexpr double radius = 6367470.;

    static constexpr double a = radius;
    static constexpr double b = radius;
};


struct DatumGrs80 {
    static constexpr double a = 6378137.;
    static constexpr double b = 6356752.314140;
};


struct DatumWgs84 {
    static constexpr double a = 6378137.;
    static constexpr double b = 6356752.314245;
};


using Earth = SphereT<DatumIFS>;
using Grs80 = OblateSpheroidT<DatumGrs80>;
using Wgs84 = OblateSpheroidT<DatumWgs84>;


extern const Earth EARTH;
extern const Grs80 GRS80;
extern const Wgs84 WGS84;


}  // namespace eckit::geo::figure
