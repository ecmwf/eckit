// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/figure/Earth.h"

#include "eckit/geo/Figure.h"


namespace eckit::geo::figure {


static const FigureRegisterType<Earth> REGISTER1("earth");
static const FigureRegisterType<EarthGrib1> REGISTER2("grib1");
static const FigureRegisterType<EarthGrs80> REGISTER3("grs80");
static const FigureRegisterType<EarthWgs84> REGISTER4("wgs84");
static const FigureRegisterType<EarthIau1965> REGISTER5("iau1965");
static const FigureRegisterType<EarthWgs84Sphere> REGISTER6("wgs84_sphere");


const Earth EARTH;
const EarthGrib1 EARTH_GRIB1;
const EarthGrs80 EARTH_GRS80;
const EarthIau1965 EARTH_IAU1965;
const EarthWgs84 EARTH_WGS84;
const EarthWgs84Sphere EARTH_WGS84_SPHERE;


}  // namespace eckit::geo::figure
