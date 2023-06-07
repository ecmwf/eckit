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


}  // namespace eckit::geo::grid
