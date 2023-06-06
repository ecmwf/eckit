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


#include "eckit/geo/grid/LambertAzimuthalEqualArea.h"

#include <cmath>

#include "eckit/geo/Iterator.h"


namespace eckit::geo::grid {


static const GridBuilder<LambertAzimuthalEqualArea> __builder("lambert_azimuthal_equal_area");


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Configuration& param) :
    RegularGrid(param, make_projection(param)) {}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Projection& projection, const BoundingBox& bbox,
                                                     const LinearSpacing& x, const LinearSpacing& y,
                                                     const Shape& shape) :
    RegularGrid(projection, bbox, x, y, shape) {}


Projection LambertAzimuthalEqualArea::make_projection(const Configuration& param) {
    auto spec = make_proj_spec(param);
    if (!spec.empty()) {
        return spec;
    }

    double standardParallel = 0.;
    double centralLongitude = 0.;
    double radius           = 0.;
    ASSERT(param.get("standardParallelInDegrees", standardParallel));
    ASSERT(param.get("centralLongitudeInDegrees", centralLongitude));
    param.get("radius", radius = Earth::radius());

    return Configuration("type", "lambert_azimuthal_equal_area")
        .set("standard_parallel", standardParallel)
        .set("central_longitude", centralLongitude)
        .set("radius", radius);
}


const Grid* LambertAzimuthalEqualArea::croppedGrid(const BoundingBox& bbox) const {
    auto mm = minmax_ij(bbox);
    auto Ni = x_.size();

    auto projection = grid_.projection();
    ASSERT(projection);

    auto first = [this, projection, Ni](size_t firsti, size_t firstj) -> Point2 {
        for (std::unique_ptr<Iterator> it(iterator()); it->next();) {
            auto i = it->index() % Ni;
            auto j = it->index() / Ni;
            if (i == firsti && j == firstj) {
                const auto& latlon = *(*it);
                return projection.xy(PointLonLat{latlon[1], latlon[0]});
            }
        }
        throw UserError("LambertAzimuthalEqualArea::croppedGrid: cannot find first point");
    }(mm.first.i, mm.first.j);

    auto x = linspace(first.x(), std::abs(x_.step()), long(mm.second.i - mm.first.i + 1), xPlus_);
    auto y = linspace(first.y(), std::abs(y_.step()), long(mm.second.j - mm.first.j + 1), yPlus_);

    return new LambertAzimuthalEqualArea(projection, bbox, x, y, shape_);
}


}  // namespace eckit::geo::grid
