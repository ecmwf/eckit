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


#include "eckit/geo/figure/Sphere.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::figure {


Sphere::Sphere(double R) : R_(R) {
    ASSERT_MSG(types::is_strictly_greater(R_, 0.), "Sphere requires R > 0");
}


Sphere::Sphere(const Spec& spec) : Sphere(spec.get_double("R")) {}


double Sphere::area(const area::BoundingBox& bbox) const {
    auto lonf = bbox.isPeriodicWestEast() ? 1. : ((bbox.east - bbox.west) / PointLonLat::GLOBE);
    ASSERT(0. <= lonf && lonf <= 1.);

    const auto sn = std::sin(bbox.north * util::DEGREE_TO_RADIAN);
    const auto ss = std::sin(bbox.south * util::DEGREE_TO_RADIAN);

    auto latf = 0.5 * (sn - ss);
    ASSERT(0. <= latf && latf <= 1.);

    return 4. * M_PI * R_ * R_ * latf * lonf;
}


}  // namespace eckit::geo::figure
