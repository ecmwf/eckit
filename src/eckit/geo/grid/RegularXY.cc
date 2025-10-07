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


bool RegularXY::Increments::operator==(const Increments& other) const {
    return types::is_approximately_equal(dx, other.dx) && types::is_approximately_equal(dy, other.dy);
}


static RegularXY::Increments make_increments_from_spec(const Spec& spec) {
    std::vector<RegularXY::Increments::value_type> grid(2);

    if (spec.get("dx", grid[0]) && spec.get("dy", grid[1])) {
        return {grid[0], grid[1]};
    }

    if (spec.get("grid", grid) && grid.size() == 2) {
        return {grid[0], -grid[1]};
    }

    throw exception::SpecError("'grid' = ['dx', 'dy'] expected", Here());
}


RegularXY::RegularXY(const Spec& spec) :
    Regular(make_ranges_from_spec(spec), area::BoundingBox(spec), ProjectionFactory::build(spec)) {}


Regular::Ranges RegularXY::make_ranges_from_spec(const Spec& spec) {
    auto inc = make_increments_from_spec(spec);
    Shape shape(spec);

    std::unique_ptr<const Projection> projection(ProjectionFactory::build(spec));

    std::vector<PointLonLat::value_type> v(2);
    auto first_lonlat(
        (spec.get("first_lon", v[0]) && spec.get("first_lat", v[1])) || (spec.get("first_lonlat", v) && v.size() == 2)
            ? PointLonLat{v[0], v[1]}
            : throw exception::SpecError("['first_lonlat' = ['first_lon', 'first_lat'] expected", Here()));

    PointXY a = std::get<PointXY>(projection->inv(first_lonlat));
    PointXY b{a.X + (shape.nx > 1 ? inc.dx * static_cast<double>(shape.nx - 1) : 0),  //
              a.Y - (shape.ny > 1 ? inc.dy * static_cast<double>(shape.ny - 1) : 0)};

    return {range::Regular::make_xy_range(inc.dx, a.X, b.X, a.X), range::Regular::make_xy_range(inc.dy, a.Y, b.Y, a.Y)};
}


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
