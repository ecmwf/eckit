// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/figure/Earth.h"


namespace eckit::geo::figure {


static const FigureBuilder<Earth> REGISTER1("earth");
static const FigureBuilder<EarthGrib1> REGISTER2("grib1");
static const FigureBuilder<EarthGrs80> REGISTER3("grs80");
static const FigureBuilder<EarthWgs84> REGISTER4("wgs84");
static const FigureBuilder<EarthIau1965> REGISTER5("iau1965");
static const FigureBuilder<EarthWgs84Sphere> REGISTER6("wgs84_sphere");


const Earth EARTH;
const EarthGrib1 EARTH_GRIB1;
const EarthGrs80 EARTH_GRS80;
const EarthIau1965 EARTH_IAU1965;
const EarthWgs84 EARTH_WGS84;
const EarthWgs84Sphere EARTH_WGS84_SPHERE;


}  // namespace eckit::geo::figure
