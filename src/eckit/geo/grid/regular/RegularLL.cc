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


#include "eckit/geo/grid/regular/RegularLL.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Increments.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/regex.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(Increments{spec}, area::BoundingBox{spec}, [&spec]() -> PointLonLat {
        if (double lon = 0., lat = 0.; spec.get("reference_lon", lon) && spec.get("reference_lat", lat)) {
            return {lon, lat};
        }

        area::BoundingBox area{spec};
        return {area.west, area.south};
    }()) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox) :
    RegularLL(inc, bbox, {bbox.south, bbox.west}) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, const PointLonLat& ref) :
    Regular(bbox),
    lon_(inc.west_east, bbox.west, bbox.east, ref.lon),
    lat_(inc.south_north, bbox.north, bbox.south, ref.lat) {
    ASSERT(size() > 0);
}


Grid::iterator RegularLL::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator RegularLL::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


void RegularLL::spec(spec::Custom& custom) const {
    ASSERT(!custom.has("grid"));
    custom.set("grid", std::vector<double>{lon_.increment(), lat_.increment()});
}


#define POSITIVE_REAL "[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][-+][0-9]+)?"
static const std::string __pattern("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");
#undef POSITIVE_REAL


Spec* RegularLL::spec(const std::string& name) {

    auto match = util::regex_match(__pattern, name);
    ASSERT(match);
    ASSERT(match.size() == 9);  // because of sub-matches

    auto d = Translator<std::string, double>{};

    return new spec::Custom({{"type", "regular_ll"}, {"grid", std::vector<double>{d(match[1]), d(match[5])}}});
}


static const GridRegisterType<RegularLL> __grid_type("regular_ll");
static const GridRegisterName<RegularLL> __grid_pattern(__pattern);


}  // namespace eckit::geo::grid::regular
