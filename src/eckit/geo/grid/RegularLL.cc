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
#include "eckit/geo/projection/EquidistantCylindrical.h"
#include "eckit/geo/projection/Reverse.h"
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


#define POSITIVE_REAL "[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)(e[-+][0-9]+)?"
static const std::string REGULAR_LL_PATTERN("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");
#undef POSITIVE_REAL


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(
        Increments{spec}, area::BoundingBox{spec},
        [&spec]() -> PointLonLat {
            std::vector<PointLonLat::value_type> v(2);
            if (spec.get("reference_lon", v[0]) && spec.get("reference_lat", v[1])) {
                return {v[0], v[1]};
            }

            if (spec.get("reference_lonlat", v) && v.size() == 2) {
                return {v[0], v[1]};
            }

            return {0, 0};
        }(),
        spec.has("projection") ? Projection::make_from_spec(spec)
                               : new projection::Reverse<projection::EquidistantCylindrical>) {
    ASSERT(!empty());
}


RegularLL::RegularLL(const Increments& inc, Projection* proj) : RegularLL(inc, area::BoundingBox{}, {0, 0}, proj) {}


RegularLL::RegularLL(const Increments& inc, area::BoundingBox bbox, PointLonLat ref, Projection* proj) :
    Regular({new range::RegularLongitude(inc.dx, bbox.west, bbox.east, ref.lon),
             new range::RegularLatitude(inc.dy, bbox.north, bbox.south, ref.lat)},
            bbox, proj == nullptr ? new projection::Reverse<projection::EquidistantCylindrical> : proj) {
    ASSERT(!empty());
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

    boundingBox().fill_spec(custom);
}


const std::string& RegularLL::type() const {
    static const std::string type{"regular-ll"};
    return type;
}


Point RegularLL::first_point() const {
    ASSERT(!empty());
    return PointLonLat{x().values().front(), y().values().front()};  // First longitude and first latitude
}


Point RegularLL::last_point() const {
    ASSERT(!empty());
    return PointLonLat{x().values().back(), y().values().back()};
}


std::pair<std::vector<double>, std::vector<double>> RegularLL::to_latlons() const {
    const auto N = size();

    std::pair<std::vector<double>, std::vector<double>> latlon;
    auto& lat = latlon.first;
    auto& lon = latlon.second;
    lat.reserve(N);
    lon.reserve(N);

    for (auto point : *this) {
        const auto& p = std::get<PointLonLat>(point);
        lat.emplace_back(p.lat);
        lon.emplace_back(p.lon);
    }

    return latlon;
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
