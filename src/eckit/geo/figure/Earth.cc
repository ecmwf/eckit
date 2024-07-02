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


static const ConcreteBuilderT1<Figure, Earth> REGISTER1("earth");
static const ConcreteBuilderT1<Figure, GRS80> REGISTER2("grs80");
static const ConcreteBuilderT1<Figure, WGS84> REGISTER3("wgs84");


}  // namespace eckit::geo::figure
