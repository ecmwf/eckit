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


#include "eckit/geo/grid/regular/RegularXY.h"

#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"

namespace eckit::geo::grid::regular {


RegularXY::Increments::Increments(value_type dx, value_type dy) : array{dx, dy} {
    if (dx < 0 || dy < 0) {
        throw exception::SpecError("'grid' = ['dx', 'dy'] must be positive", Here());
    }
}


bool RegularXY::Increments::operator==(const Increments& other) const {
    return types::is_approximately_equal(dx, other.dx) && types::is_approximately_equal(dy, other.dy);
}


RegularXY::Increments RegularXY::Increments::make_from_spec(const Spec& spec) {
    std::vector<RegularXY::Increments::value_type> grid(2);

    if (spec.get("dx", grid[0]) && spec.get("dy", grid[1])) {
        return {grid[0], grid[1]};
    }

    if (spec.get("grid", grid) && grid.size() == 2) {
        return {grid[0], -grid[1]};
    }

    throw exception::SpecError("'grid' = ['dx', 'dy'] expected", Here());
}


RegularXY::RangeXY::RangeXY(double start, double stop_included, double step) :
    RegularXY{step, start,
              stop_included +
                  [](double step) {
                      return types::is_strictly_greater(step, 0.)   ? PointXY::EPS
                             : types::is_strictly_greater(0., step) ? -PointXY::EPS
                                                                    : 0;
                  }(step),
              start} {}


RegularXY::RangeXY RegularXY::RangeXY::make_from_spec(const Spec& spec, const std::string& key) {
    if (std::vector<double> r(3);
        (spec.get(key + "_start", r[0]) && spec.get(key + "_stop", r[1]) && spec.get(key + "_step", r[2])) ||
        (spec.get(key, r) && r.size() == 3)) {
        return RangeXY{r[0], r[1], r[2]};
    }

    throw exception::SpecError("'" + key + "' = ['start', 'stop', 'step'] expected", Here());
}


RegularXY::RegularXY(const Spec& spec) :
    RegularXY(Increments::make_from_spec(spec), BoundingBoxXY(spec), order::Scan{spec},
              Projection::make_from_spec(spec)) {}


RegularXY::RegularXY(const Increments& inc, BoundingBoxXY bbox, order::Scan s, Projection* p) :
    Regular(s, p),
    x_(s.is_scan_i_positive() ? inc.dx : -inc.dx, s.is_scan_i_positive() ? bbox.min_x : bbox.max_x,
       s.is_scan_i_positive() ? bbox.max_x : bbox.min_x),
    y_(s.is_scan_j_positive() ? inc.dy : -inc.dy, s.is_scan_j_positive() ? bbox.min_y : bbox.max_y,
       s.is_scan_j_positive() ? bbox.max_y : bbox.min_y) {
    ASSERT(!empty());
}


RegularXY::RegularXY(const RangeXY& x, const RangeXY& y, Projection* p) :
    Regular(order::Scan("i" + std::string(x.b() - x.a() < 0 ? "-" : "+") +  //
                        "j" + std::string(y.b() - y.a() < 0 ? "-" : "+")),
            p),
    x_(x),
    y_(y) {
    ASSERT(!empty());
}


const std::string& RegularXY::type() const {
    NOTIMP;
}


Point RegularXY::first_point() const {
    ASSERT(!empty());
    return projection().inv(PointXY{x().values().front(), y().values().front()});
}


Point RegularXY::last_point() const {
    ASSERT(!empty());
    return projection().inv(PointXY{x().values().back(), y().values().back()});
}


Grid::BoundingBox* RegularXY::calculate_bbox() const {
    return area::BoundingBox::make_from_projection(PointXY{x_.min(), y_.min()}, PointXY{x_.max(), y_.max()},
                                                   projection())
        .release();
}


void RegularXY::fill_spec(spec::Custom& custom) const {
    Regular::fill_spec(custom);

    custom.set("grid", std::vector<double>{dx(), dy()});
    custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});
    // custom.set("first_lonlat", std::vector<double>{first_lonlat.lon, first_lonlat.lat});
}


// ---


// static const GridRegisterType<RegularXY> GRID2("lambert");
// static const GridRegisterType<RegularXY> GRID3("lambert_lam");


}  // namespace eckit::geo::grid::regular
