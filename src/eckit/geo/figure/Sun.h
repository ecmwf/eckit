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

#include "eckit/geo/figure/SphereT.h"


namespace eckit::geo::figure {


struct DatumSun {
    static constexpr double radius = 695990000.;

    static constexpr double a = radius;
    static constexpr double b = radius;

    static constexpr bool default_figure = false;
};


using Sun = SphereT<DatumSun>;


extern const Sun SUN;


}  // namespace eckit::geo::figure
