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


#include "eckit/geo/projection/XYToLonLat.h"

#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<XYToLonLat> PROJECTION1("xy_to_ll");
static ProjectionRegisterType<XYToLonLat> PROJECTION2("plate-carree");


const std::string& XYToLonLat::type() const {
    static const std::string type{"xy-to-ll"};
    return type;
}


void XYToLonLat::fill_spec(spec::Custom& custom) const {
    custom.set("projection", "xy_to_ll");
}


}  // namespace eckit::geo::projection
