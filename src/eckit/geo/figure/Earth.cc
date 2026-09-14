// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/figure/Earth.h"


namespace eckit::geo::figure {


static const FigureBuilder<Earth> REGISTER1("earth");
static const FigureBuilder<Grs80> REGISTER2("grs80");
static const FigureBuilder<Wgs84> REGISTER3("wgs84");


const Earth EARTH;
const Grs80 GRS80;
const Wgs84 WGS84;


}  // namespace eckit::geo::figure
