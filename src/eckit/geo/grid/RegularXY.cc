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


#include "eckit/geo/grid/RegularXY.h"

#include <memory>

#include "eckit/geo/Projection.h"
#include "eckit/geo/Shape.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::grid {


namespace {


Range* make_x_range(const Spec& spec) {
    auto inc   = RegularXY::make_increments_from_spec(spec);
    auto first = RegularXY::make_first_point_from_spec(spec);
    Shape shape(spec);

    auto a = std::get<PointXY>(std::unique_ptr<const Projection>(ProjectionFactory::build(spec))->inv(first)).X;
    auto b = a + (shape.nx > 1 ? inc.dx * static_cast<double>(shape.nx - 1) : 0);

    return range::Regular::make_xy_range(inc.dx, a, b, a);
}


Range* make_y_range(const Spec& spec) {
    auto inc   = RegularXY::make_increments_from_spec(spec);
    auto first = RegularXY::make_first_point_from_spec(spec);
    Shape shape(spec);

    auto a = std::get<PointXY>(std::unique_ptr<const Projection>(ProjectionFactory::build(spec))->inv(first)).Y;
    auto b = a + (shape.ny > 1 ? inc.dy * static_cast<double>(shape.ny - 1) : 0);

    return range::Regular::make_xy_range(inc.dy, a, b, a);
}


}  // namespace


bool RegularXY::Increments::operator==(const Increments& other) const {
    return types::is_approximately_equal(dx, other.dx) && types::is_approximately_equal(dy, other.dy);
}


RegularXY::Increments RegularXY::make_increments_from_spec(const Spec& spec) {
    std::vector<RegularXY::Increments::value_type> grid(2);

    if (spec.get("dx", grid[0]) && spec.get("dy", grid[1])) {
        return {grid[0], grid[1]};
    }

    if (spec.get("grid", grid) && grid.size() == 2) {
        return {grid[0], -grid[1]};
    }

    throw exception::SpecError("'grid' = ['dx', 'dy'] expected", Here());
}


PointLonLat RegularXY::make_first_point_from_spec(const Spec& spec) {
    std::vector<PointLonLat::value_type> v(2);

    if (spec.get("first_lon", v[0]) && spec.get("first_lat", v[1])) {
        return {v[0], v[1]};
    }

    if (spec.get("first_lonlat", v) && v.size() == 2) {
        return {v[0], v[1]};
    }

    throw exception::SpecError("['first_lonlat' = ['first_lon', 'first_lat'] expected", Here());
}


RegularXY::RegularXY(const Spec& spec) : Regular(make_x_range(spec), make_y_range(spec)) {}


const std::string& RegularXY::type() const {
    return projection().type();
}


Point RegularXY::first_point() const {
    ASSERT(!empty());
    return PointXY{x().values().front(), y().values().front()};
}


Point RegularXY::last_point() const {
    ASSERT(!empty());
    return PointXY{x().values().back(), y().values().back()};
}


void RegularXY::fill_spec(spec::Custom& custom) const {
    Regular::fill_spec(custom);

    custom.set("grid", std::vector<double>{dx(), dy()});
    custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});
    custom.set("first_lonlat", std::vector<double>{first_lonlat.lon, first_lonlat.lat});
}


static const GridRegisterType<RegularXY> GRID1("lambert");
static const GridRegisterType<RegularXY> GRID2("lambert_lam");


}  // namespace eckit::geo::grid
