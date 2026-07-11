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


#include <vector>

#include "eckit/geo/grid/regular/RegularXY.h"
#include "eckit/geo/projection/PROJ.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid::regular {


struct SwissLV95 : RegularXY {
    explicit SwissLV95(const Spec& spec) :               //
        RegularXY(RangeXY::make_from_spec(spec, "x"),    //
                  RangeXY::make_from_spec(spec, "y"),    //
                  new projection::PROJ("EPSG:2056")) {}  // https://epsg.io/2056
};


struct SwissLV95ByName : SwissLV95 {
    using SwissLV95::SwissLV95;
    static Spec* spec(const std::string& name) {
        return new spec::Custom({{"type", name},
                                 {"x", std::vector<double>{2439000., 2867000., 1000.}},  //
                                 {"y", std::vector<double>{1040500., 1333500., 1000.}}});
    }
};


static const GridRegisterType<SwissLV95> SWISSLV95("swisslv95");
static const GridRegisterName<SwissLV95ByName> SWISSLV95_BY_NAME("swisslv95");


}  // namespace eckit::geo::grid::regular
