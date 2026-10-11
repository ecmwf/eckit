// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/regular/RegularXY.h"

#include <string>
#include <tuple>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/area/BoundingBoxXY.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/projection/None.h"
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
        throw exception::SpecError("RegularXY: 'x' and 'y', or 'grid' = ['dx', 'dy'] (positive) expected", Here());
    }

    const area::BoundingBoxXY bbox(spec);
    const order::Scan scan(spec);

    const auto [min, max, step, positive] =
        key == "x" ? std::tuple{bbox.min_x(), bbox.max_x(), grid[0], scan.is_scan_i_positive()}
                   : std::tuple{bbox.min_y(), bbox.max_y(), grid[1], scan.is_scan_j_positive()};

    return positive ? RangeXY{min, max, step} : RangeXY{max, min, -step};
}


PointXY false_from_spec(const Spec& spec) {
    if (std::vector<double> f; spec.get("false", f)) {
        if (f.size() != 2) {
            throw exception::SpecError("RegularXY: 'false' = ['x', 'y'] expected", Here());
        }
        return {f[0], f[1]};
    }

    return {spec.get_double("false_x", 0.), spec.get_double("false_y", 0.)};
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

    throw exception::SpecError("RegularXY: '" + key + "' = ['start', 'stop', 'step'] expected", Here());
}


RegularXY::RegularXY(const Spec& spec) :
    RegularXY(range_from_spec(spec, "x"), range_from_spec(spec, "y"),
              spec.has("projection") ? ProjectionFactory::build(spec) : nullptr, false_from_spec(spec)) {
    // the order is as the directions of x and y, and can scan j first
    if (spec.has("order")) {
        order::Scan s(spec);
        if (s.is_scan_i_positive() != scan().is_scan_i_positive() ||
            s.is_scan_j_positive() != scan().is_scan_j_positive()) {
            throw exception::SpecError("RegularXY: 'order' and the directions of 'x' and 'y' differ", Here());
        }
        scan(s);
    }
}


RegularXY::RegularXY(const RangeXY& x, const RangeXY& y, const Projection* p, const PointXY& false_xy) :
    Regular(order::Scan("i" + std::string(x.b() < x.a() ? "-" : "+") + "j" + std::string(y.b() < y.a() ? "-" : "+")),
            p != nullptr ? p : new projection::None),
    x_(x),
    y_(y),
    false_(false_xy),
    projected_(p != nullptr) {
    ASSERT(!empty());

    if (projected_ && projection().source_point_coordinates() != point_coordinates<PointXY>()) {
        throw exception::SpecError("RegularXY: 'projection' from (x, y) expected (e.g. with reverse: true)", Here());
    }
}


const std::string& RegularXY::type() const {
    static const std::string type{"regular_xy"};
    return type;
}


Point RegularXY::first_point() const {
    ASSERT(!empty());
    return point(x_.values().front(), y_.values().front());
}


Point RegularXY::last_point() const {
    ASSERT(!empty());
    return point(x_.values().back(), y_.values().back());
}


Grid::BoundingBox* RegularXY::calculate_bbox() const {
    if (!projected_) {
        return new BoundingBox;  // (x, y) points, not geographic
    }

    return area::BoundingBox::make_from_projection(PointXY{x_.min(), y_.min()} - false_,
                                                   PointXY{x_.max(), y_.max()} - false_, projection())
        .release();
}


Point RegularXY::point(double x, double y) const {
    const PointXY p{x, y};
    return projected_ ? projection().fwd(p - false_) : p;
}


void RegularXY::fill_spec(spec::Custom& custom) const {
    custom.set("x", range_spec(x_));
    custom.set("y", range_spec(y_));

    if (false_.X() != 0. || false_.Y() != 0.) {
        custom.set("false", std::vector<double>{false_.X(), false_.Y()});
    }

    if (projected_) {
        spec::Custom projection_spec;
        projection().fill_spec(projection_spec);
        custom.set("projection", projection_spec);
    }

    if (scan().order() != order::Scan::order_default()) {
        custom.set("order", scan().order());
    }
}


static const GridRegisterType<RegularXY> GRIDTYPE("regular_xy");


}  // namespace eckit::geo::grid::regular
