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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


#define POSITIVE_REAL "[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)(e[-+][0-9]+)?"
static const std::string REGULAR_LL_PATTERN("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");
#undef POSITIVE_REAL


bool RegularLL::Increments::operator==(const Increments& other) const {
    return types::is_approximately_equal(dlon, other.dlon) && types::is_approximately_equal(dlat, other.dlat);
}


RegularLL::Increments RegularLL::make_increments_from_spec(const Spec& spec) {
    std::vector<RegularLL::Increments::value_type> grid(2);

    if (spec.get("dlon", grid[0]) && spec.get("dlat", grid[1])) {
        return {grid[0], grid[1]};
    }

    if (spec.get("grid", grid) && grid.size() == 2) {
        return {grid[0], grid[1]};
    }

    throw exception::SpecError("'grid' = ['dlon', 'dlat'] expected", Here());
}


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(make_increments_from_spec(spec), BoundingBox{spec}, [&spec]() {
        area::BoundingBox bbox{spec};
        return PointLonLat::make_from_spec(spec, "reference", {bbox.west, bbox.south});
    }()) {}


RegularLL::RegularLL(const Increments& inc) : RegularLL(inc, BoundingBox{}, {0, 0}) {}


RegularLL::RegularLL(const Increments& inc, BoundingBox bbox, PointLonLat ref) :
    x_(*range::RegularLongitudeRange(inc.dlon, bbox.west, bbox.east, ref.lon).make_cropped_range(bbox.west, bbox.east)),
    y_(*range::RegularLatitudeRange(-inc.dlat, bbox.north, bbox.south, ref.lat)
            .make_cropped_range(bbox.north, bbox.south)) {
    ASSERT(!empty());

    order(std::string{x().increment() < 0 ? "i-" : "i+"} +  //
          std::string{y().increment() < 0 ? "j-" : "j+"});

    // TODO reference to modify bounding box
}


Spec* RegularLL::spec(const std::string& name) {
    std::smatch match;
    std::regex_match(name, match, std::regex(REGULAR_LL_PATTERN));
    ASSERT(match.size() == 9);

    auto d = Translator<std::string, double>{};

    return new spec::Custom({{"type", "regular_ll"}, {"grid", std::vector<double>{d(match[1]), d(match[5])}}});
}


void RegularLL::fill_spec(spec::Custom& custom) const {
    custom.set("grid", std::vector<double>{std::abs(dlon()), std::abs(dlat())});

    if (auto o = order(); o != order::Scan::order_default()) {
        custom.set("order", o);
    }

    auto shift = [](Fraction inc, Fraction f) { return (f / inc).decimalPart() * inc; };
    if (PointLonLat reference{shift(x_.af(), x_.increment()), shift(y_.af(), y_.increment())};
        !points_equal(reference, PointLonLat{})) {
        custom.set("reference", std::vector<double>{reference.lon, reference.lat});
    }

    if (auto bbox = boundingBox(); bbox != BoundingBox::bounding_box_default()) {
        auto [n, w, s, e] = bbox.deconstruct();
        custom.set("area", std::vector<double>{n, w, s, e});
    }
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
        return new RegularLL({dlon(), dlat()}, cropped);
    }

    throw UserError("RegularLL: cannot crop grid (empty intersection)", Here());
}


Grid::BoundingBox* RegularLL::calculate_bbox() const {
    auto n = y_.includesNorthPole() ? PointLonLat::RIGHT_ANGLE : y_.max();
    auto s = y_.includesSouthPole() ? -PointLonLat::RIGHT_ANGLE : y_.min();
    auto w = x_.a();
    auto e = x_.periodic() ? w + PointLonLat::FULL_ANGLE : x_.b();

    return new BoundingBox{n, w, s, e};
}


static const GridRegisterName<RegularLL> GRIDNAME(REGULAR_LL_PATTERN);

static const GridRegisterType<RegularLL> GRIDTYPE1("regular_ll");
static const GridRegisterType<RegularLL> GRIDTYPE2("rotated_ll");


}  // namespace eckit::geo::grid
