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
