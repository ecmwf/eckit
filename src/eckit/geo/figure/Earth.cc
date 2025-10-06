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


#include "eckit/geo/figure/Earth.h"


namespace eckit::geo::figure {


static const FigureBuilder<Earth> REGISTER1("earth");
static const FigureBuilder<Grs80> REGISTER2("grs80");
static const FigureBuilder<Wgs84> REGISTER3("wgs84");


const Earth EARTH;
const Grs80 GRS80;
const Wgs84 WGS84;


}  // namespace eckit::geo::figure
