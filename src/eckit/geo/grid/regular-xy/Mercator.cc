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


#include "eckit/geo/grid/regular-xy/Mercator.h"

#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid::regularxy {


static const GridRegisterType<Mercator> GRIDTYPE("mercator");


void Mercator::fill_spec(spec::Custom& custom) const {
    RegularXY::fill_spec(custom);

    custom.set("type", "mercator");

    // FIXME a lot more stuff to add here!
    //...
}


const std::string& Mercator::type() const {
    static const std::string type{"regular-xy-mercator"};
    return type;
}


}  // namespace eckit::geo::grid::regularxy
