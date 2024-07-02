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


static ProjectionBuilder<XYToLonLat> PROJECTION1("xy_to_ll");
static ProjectionBuilder<XYToLonLat> PROJECTION2("plate-carree");


void XYToLonLat::fill_spec(spec::Custom& custom) const {
    custom.set("projection", "ll_to_xy");
}


}  // namespace eckit::geo::projection
