// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/regular/RegularXY.h"

#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/area/BoundingBoxXY.h"
#include "eckit/geo/order/RegularScan.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"

namespace eckit::geo::grid::regular {


namespace {


using RangeXY = RegularXY::RangeXY;
using Spec    = RegularXY::Spec;


/// From x (or y), or as a bounding box: from grid: [dx, dy], bounding_box_xy and order
RangeXY range_from_spec(const Spec& spec, const std::string& key) {
    if (spec.has(key)) {
        return RangeXY::make_from_spec(spec, key);
    }

    std::vector<double> grid;
    if (!spec.get("grid", grid) || grid.size() != 2 || !types::is_strictly_greater(grid[0], 0.) ||
        !types::is_strictly_greater(grid[1], 0.)) {
        throw exception::SpecError(
            "RegularXY: {x: [start, stop, step], y: [start, stop, step]} or {grid: [dx, dy]} (positive) expected",
            Here());
    }

    const area::BoundingBoxXY bbox(spec);
    const order::RegularScan scan(spec);

    const auto [min, max, step, positive] =
        key == "x" ? std::tuple{bbox.min_x(), bbox.max_x(), grid[0], scan.is_scan_i_positive()}
                   : std::tuple{bbox.min_y(), bbox.max_y(), grid[1], scan.is_scan_j_positive()};

    return positive ? RangeXY{min, max, step} : RangeXY{max, min, -step};
}


/// False easting and northing: false_xy: [x, y] (by default [0, 0], e.g. of a projection with its own), or so the
/// false origin (x, y) = (0, 0) is at false_lonlat: [lon, lat], by the projection
PointXY false_from_spec(const Spec& spec) {
    for (const std::string key : {"false", "false_x", "false_y"}) {
        if (spec.has(key)) {
            throw exception::SpecError("RegularXY: {false_xy: [x, y]} expected (not '" + key + "')", Here());
        }
    }

    const auto xy = spec.has("false_xy");
    const auto ll = spec.has("false_lonlat");

    if (!xy && !ll) {
        return {};
    }

    if (xy && !ll) {
        if (std::vector<double> f; spec.get("false_xy", f) && f.size() == 2) {
            return {f[0], f[1]};
        }
    }
    else if (!xy && spec.has("projection")) {
        const std::unique_ptr<const Projection> projection(ProjectionFactory::build(spec));
        if (projection->source_point_coordinates() != geo::point_coordinates<PointXY>() ||
            projection->target_point_coordinates() != geo::point_coordinates<PointLonLat>()) {
            throw exception::SpecError(
                "RegularXY: 'projection' from (x, y) to (lon, lat) expected (e.g. with reverse: true)", Here());
        }

        // the false origin less the false easting/northing is the projected false_lonlat
        return -std::get<PointXY>(projection->inv(PointLonLat::make_from_spec(spec, "false_lonlat")));
    }

    throw exception::SpecError(
        "RegularXY: {false_xy: [x, y]} or {false_lonlat: [lon, lat], projection: {...}} expected", Here());
}


std::vector<double> range_spec(const range::RegularXY& range) {
    return {range.a(), range.b(), range.inc()};
}


}  // namespace


RegularXY::RangeXY::RangeXY(double start, double stop_included, double step) : RegularXY{step, start, stop_included} {}


RegularXY::RangeXY RegularXY::RangeXY::make_from_spec(const Spec& spec, const std::string& key) {
    if (std::vector<double> r; spec.get(key, r) && r.size() == 3) {
        return RangeXY{r[0], r[1], r[2]};
    }

    throw exception::SpecError("RegularXY: {" + key + ": [start, stop, step]} expected", Here());
}


RegularXY::RegularXY(const Spec& spec) :
    RegularXY(range_from_spec(spec, "x"), range_from_spec(spec, "y"),
              spec.has("projection") ? projection_from_spec(spec) : nullptr, false_from_spec(spec)) {
    // the order is as the directions of x and y, and can scan j first
    if (spec.has("order")) {
        order::RegularScan s(spec);
        if (s.is_scan_i_positive() != scan().is_scan_i_positive() ||
            s.is_scan_j_positive() != scan().is_scan_j_positive()) {
            throw exception::SpecError("RegularXY: 'order' and the directions of 'x' and 'y' differ", Here());
        }
        scan(s);
    }
}


RegularXY::RegularXY(const RangeXY& x, const RangeXY& y, std::unique_ptr<const Projection> p, const PointXY& false_xy) :
    RegularXY(x, y, p != nullptr, std::move(p), false_xy) {}


RegularXY::RegularXY(const RangeXY& x, const RangeXY& y, bool projected, std::unique_ptr<const Projection>&& p,
                     const PointXY& false_xy) :
    Regular(
        order::RegularScan("i" + std::string(x.b() < x.a() ? "-" : "+") + "j" + std::string(y.b() < y.a() ? "-" : "+")),
        std::move(p)),
    x_(x),
    y_(y),
    false_(false_xy),
    projected_(projected) {
    if (empty()) {
        throw exception::GridError("RegularXY: no points", Here());
    }

    if (projected_ && projection().source_point_coordinates() != geo::point_coordinates<PointXY>()) {
        throw exception::SpecError("RegularXY: 'projection' from (x, y) expected (e.g. with reverse: true)", Here());
    }

    // without a projection, the points are as given, so a false easting/northing would be in the spec only
    if (!projected_ && (false_.X() != 0. || false_.Y() != 0.)) {
        throw exception::SpecError("RegularXY: {false_xy: [x, y], projection: {...}} expected", Here());
    }
}


const std::string& RegularXY::type() const {
    static const std::string type{"regular_xy"};
    return type;
}


Grid::BoundingBox* RegularXY::calculate_bbox() const {
    if (!projected_) {
        return new BoundingBox;  // (x, y) points, not geographic
    }

    return area::BoundingBox::make_from_projection(PointXY{x_.min(), y_.min()} - false_,
                                                   PointXY{x_.max(), y_.max()} - false_, projection())
        .release();
}


Point RegularXY::point_xy(double x, double y) const {
    const PointXY p{x, y};
    return projected_ ? projection().fwd(p - false_) : p;
}


void RegularXY::fill_spec(spec::Custom& custom) const {
    custom.set("x", range_spec(x_));
    custom.set("y", range_spec(y_));

    if (false_.X() != 0. || false_.Y() != 0.) {
        custom.set("false_xy", std::vector<double>{false_.X(), false_.Y()});
    }

    if (projected_) {
        projection().fill_grid_spec(custom);
    }

    if (scan().order() != order::RegularScan::order_default()) {
        custom.set("order", scan().order());
    }
}


static const GridRegisterType<RegularXY> GRIDTYPE("regular_xy");


}  // namespace eckit::geo::grid::regular
