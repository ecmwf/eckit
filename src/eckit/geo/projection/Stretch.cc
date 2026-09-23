// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/projection/Stretch.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<Stretch> PROJECTION("stretch");


Stretch::Stretch(double c) : c_(c) {
    if (types::is_approximately_equal(c_, 0.)) {
        throw exception::ProjectionError("Stretch: stretching_factor != 0", Here());
    }
    ASSERT(c_ != 0.);
}


Stretch::Stretch(const Spec& spec) : Stretch(spec.get_double("stretching_factor")) {}


const std::string& Stretch::type() const {
    static const std::string type{"stretch"};
    return type;
}


double Stretch::stretch(double a, double c) {
    auto ar = util::DEGREE_TO_RADIAN * a;
    ar      = std::asin(std::cos(2. * std::atan(c * std::tan(std::acos(std::sin(ar)) * 0.5))));
    return util::RADIAN_TO_DEGREE * ar;
}


void Stretch::fill_spec(spec::Custom& spec) const {
    spec.set("type", "stretch");
    spec.set("stretching_factor", c_);
}


}  // namespace eckit::geo::projection
