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

#include "eckit/geo/Increments.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/Shape.h"
#include "eckit/geo/range/RegularCartesian.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::grid {


Regular::Ranges RegularXY::make_ranges_from_spec(const Spec& spec) {
#if 0
    Increments inc(spec);
    Shape shape(spec);

    std::unique_ptr<Projection> projection(ProjectionFactory::instance().get("proj").create(spec));

    std::vector<PointLonLat::value_type> v(2);
    auto first_lonlat((spec.get("first_lon", v[0]) && spec.get("first_lat", v[1]))
                   || (spec.get("first_lonlat", v) && v.size() == 2)
               ? PointLonLat{v[0], v[1]}
               : throw SpecNotFound("['first_lonlat' = ['first_lon', 'first_lat'] expected", Here()));

    // TODO;


    Point2 a = std::get<Point2>(projection->inv(first_lonlat));
    Point2 b{a.X + inc.dx * static_cast<double>(shape.nx - 1), a.Y - inc.dy * static_cast<double>(shape.ny - 1)};

    return {new range::RegularCartesian(shape.nx, a.X, b.X), new range::RegularCartesian(shape.ny, a.Y, b.Y)};
#else
    return {new range::RegularCartesian(11, 0, 10), new range::RegularCartesian(11, 0, 10)};
#endif
}


void RegularXY::fill_spec(spec::Custom& custom) const {
    Regular::fill_spec(custom);

    custom.set("grid", std::vector<double>{dx(), dy()});
    custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});
    custom.set("first_lonlat", std::vector<double>{first_lonlat.lon, first_lonlat.lat});
}


}  // namespace eckit::geo::grid
