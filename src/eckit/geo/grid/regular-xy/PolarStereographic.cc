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


#include "eckit/geo/grid/regular-xy/PolarStereographic.h"

#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid::regularxy {


static const GridRegisterType<PolarStereographic> GRIDTYPE("polar_stereographic");


void PolarStereographic::fill_spec(spec::Custom& custom) const {
    RegularXY::fill_spec(custom);

    custom.set("type", "polar_stereographic");

    // FIXME a lot more stuff to add here!
    //...
}


const std::string& PolarStereographic::type() const {
    static const std::string type{"regular-xy-polar-stereographic"};
    return type;
}


}  // namespace eckit::geo::grid::regularxy
