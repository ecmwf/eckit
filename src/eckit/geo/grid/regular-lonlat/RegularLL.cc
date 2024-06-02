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


#include "eckit/geo/grid/regular-lonlat/RegularLL.h"

// #include "eckit/geo/Increments.h"
// #include "eckit/types/Fraction.h"
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regularlonlat {


#define POSITIVE_REAL "[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][-+][0-9]+)?"
static const std::string REGULAR_LL_PATTERN("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");
#undef POSITIVE_REAL


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(Increments{spec}, area::BoundingBox{spec}, [&spec]() -> PointLonLat {
        if (double lon = 0., lat = 0.; spec.get("reference_lon", lon) && spec.get("reference_lat", lat)) {
            return {lon, lat};
        }

        area::BoundingBox area{spec};
        return {area.west, area.south};
    }()) {
    ASSERT(size() > 0);
}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox) :
    RegularLL(inc, bbox, {bbox.south, bbox.west}) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, const PointLonLat& ref) :
    RegularLonLat({new range::RegularLongitude(inc.dx, bbox.west, bbox.east, ref.lon, 0.),
                   new range::RegularLatitude(inc.dy, bbox.north, bbox.south, ref.lat, 0.)},
                  bbox) {
    ASSERT(size() > 0);
}


Spec* RegularLL::spec(const std::string& name) {
    std::smatch match;
    std::regex_match(name, match, std::regex(REGULAR_LL_PATTERN));
    ASSERT(match.size() == 9);

    auto d = Translator<std::string, double>{};

    return new spec::Custom({{"type", "regular_ll"}, {"grid", std::vector<double>{d(match[1]), d(match[5])}}});
}


void RegularLL::spec(spec::Custom& custom) const {
    RegularLonLat::spec(custom);

    custom.set("grid", std::vector<double>{dx(), dy()});
}


Grid* RegularLL::make_grid_cropped(const Area& crop) const {
    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new RegularLL({dx(), dy()}, cropped);
    }

    throw UserError("RegularLL: cannot crop grid (empty intersection)", Here());
}


static const GridRegisterName<RegularLL> GRIDNAME(REGULAR_LL_PATTERN);
static const GridRegisterType<RegularLL> GRIDTYPE("regular_ll");


}  // namespace eckit::geo::grid::regularlonlat
