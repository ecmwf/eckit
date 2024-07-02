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


#include "eckit/geo/grid/RegularLL.h"

#include <algorithm>
#include <vector>

#include "eckit/geo/Increments.h"
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


#define POSITIVE_REAL "[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][-+][0-9]+)?"
static const std::string REGULAR_LL_PATTERN("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");
#undef POSITIVE_REAL


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(Increments{spec}, area::BoundingBox{spec}, projection::Rotation::make_from_spec(spec),
              [&spec]() -> PointLonLat {
                  std::vector<PointLonLat::value_type> v(2);
                  if (spec.get("reference_lon", v[0]) && spec.get("reference_lat", v[1])) {
                      return {v[0], v[1]};
                  }

                  if (spec.get("reference_lonlat", v) && v.size() == 2) {
                      return {v[0], v[1]};
                  }

                  area::BoundingBox area{spec};
                  return {area.west, area.south};
              }()) {
    ASSERT(size() > 0);
}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, projection::Rotation* rotation) :
    RegularLL(inc, bbox, rotation, {bbox.south, bbox.west}) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, projection::Rotation* rotation,
                     const PointLonLat& ref) :
    Regular({new range::RegularLongitude(inc.dx, bbox.west, bbox.east, ref.lon, 0.),
             new range::RegularLatitude(inc.dy, bbox.north, bbox.south, ref.lat, 0.)},
            rotation) {
    ASSERT(size() > 0);
}


Spec* RegularLL::spec(const std::string& name) {
    std::smatch match;
    std::regex_match(name, match, std::regex(REGULAR_LL_PATTERN));
    ASSERT(match.size() == 9);

    auto d = Translator<std::string, double>{};

    return new spec::Custom({{"type", "regular_ll"}, {"grid", std::vector<double>{d(match[1]), d(match[5])}}});
}


void RegularLL::fill_spec(spec::Custom& custom) const {
    Regular::fill_spec(custom);

    custom.set("grid", std::vector<double>{dx(), dy()});

    if (!boundingBox().global()) {
        custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});
    }

    boundingBox().fill_spec(custom);
}


Grid* RegularLL::make_grid_cropped(const Area& crop) const {
    if (auto cropped(boundingBox()); crop.intersects(cropped)) {
        return new RegularLL({dx(), dy()}, cropped);
    }

    throw UserError("RegularLL: cannot crop grid (empty intersection)", Here());
}


area::BoundingBox* RegularLL::calculate_bbox() const {
    // FIXME depends on ordering
    auto n = std::max(y().a(), y().b());
    auto s = std::min(y().a(), y().b());

    auto w = x().a();
    auto e = x().periodic() ? w + PointLonLat::FULL_ANGLE : x().b();

    return new area::BoundingBox{n, w, s, e};
}


static const GridRegisterName<RegularLL> GRIDNAME(REGULAR_LL_PATTERN);

static const GridRegisterType<RegularLL> GRIDTYPE1("regular_ll");
static const GridRegisterType<RegularLL> GRIDTYPE2("rotated_ll");


}  // namespace eckit::geo::grid
