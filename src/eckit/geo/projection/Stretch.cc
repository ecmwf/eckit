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


#include "eckit/geo/projection/Stretch.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<Stretch> PROJECTION("stretch");


Stretch::Stretch(double c) : c_(c) {
    if (types::is_approximately_equal(c_, 0.)) {
        throw ProjectionProblem("Stretch: stretching_factor != 0", Here());
    }
    ASSERT(c_ != 0.);
}


Stretch::Stretch(const Spec& spec) : Stretch(spec.get_double("stretching_factor")) {}


double Stretch::stretch(double a, double c) {
    auto ar = util::DEGREE_TO_RADIAN * a;
    ar      = std::asin(std::cos(2. * std::atan(c * std::tan(std::acos(std::sin(ar)) * 0.5))));
    return util::RADIAN_TO_DEGREE * ar;
}


void Stretch::fill_spec(spec::Custom& spec) const {
    spec.set("projection", "stretch");
    spec.set("stretching_factor", c_);
}


}  // namespace eckit::geo::projection
