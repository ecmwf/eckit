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


struct SwissLV03 : RegularXY {
    explicit SwissLV03(const Spec& spec) :                //
        RegularXY(RangeXY::make_from_spec(spec, "x"),     //
                  RangeXY::make_from_spec(spec, "y"),     //
                  new projection::PROJ("EPSG:21781")) {}  // https://epsg.io/21781
};


struct SwissLV03ByName : SwissLV03 {
    using SwissLV03::SwissLV03;
    static Spec* spec(const std::string& name) {
        return new spec::Custom({{"type", name},
                                 {"x", std::vector<double>{255500., 964500., 1000.}},  //
                                 {"y", std::vector<double>{-159500., 479500., 1000.}}});
    }
};


static const GridRegisterType<SwissLV03> SWISSLV03("swisslv03");
static const GridRegisterName<SwissLV03ByName> SWISSLV03_BY_NAME("swisslv03");


}  // namespace eckit::geo::grid::regular
