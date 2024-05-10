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


#include "eckit/geo/projection/LambertAzimuthalEqualArea.h"

#include <cmath>

#include "eckit/geo/Earth.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<LambertAzimuthalEqualArea> PROJECTION_1("lambert_azimuthal_equal_area");
static ProjectionBuilder<LambertAzimuthalEqualArea> PROJECTION_2("laea");


LambertAzimuthalEqualArea::sincos_t::sincos_t(value_type r) : array{std::sin(r), std::cos(r)} {}


LambertAzimuthalEqualArea::PointLonLatR::PointLonLatR(value_type lonr, value_type latr) : array{lonr, latr} {}


LambertAzimuthalEqualArea::PointLonLatR::PointLonLatR(const PointLonLat& p) :
    PointLonLatR{p.lon * util::DEGREE_TO_RADIAN, p.lat * util::DEGREE_TO_RADIAN} {}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Spec& spec) :
    LambertAzimuthalEqualArea({spec.get_double("lon_0"), spec.get_double("lat_0")},
                              {spec.get_double("lon_first"), spec.get_double("lat_first")}) {}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(PointLonLat centre, PointLonLat first) :
    centre_(centre),
    centre_r_(centre),
    first_(first),
    first_r_(first),
    phi0_(centre_r_.latr),
    phi_(first_r_.latr),
    dlam_(first_r_.lonr - centre_r_.lonr),
    R_(Earth::radius()) {}


Point2 LambertAzimuthalEqualArea::fwd(const PointLonLat& p) const {
    const auto kp = R_ * std::sqrt(2. / (1. + phi0_.sin * phi_.sin + phi0_.cos * phi_.cos * dlam_.cos));

    auto x = kp * phi_.cos * dlam_.sin;
    auto y = kp * (phi0_.cos * phi_.sin - phi0_.sin * phi_.cos * dlam_.cos);

    return {x, y};
}


PointLonLat LambertAzimuthalEqualArea::inv(const Point2& p) const {
    if (auto x = p.X, y = p.Y, rho = std::sqrt(x * x + y * y); !types::is_approximately_equal(rho, 0.)) {
        const sincos_t c(2. * std::asin(rho / (2. * R_)));

        const auto lonr = centre_r_.lonr + std::atan2(x * c.sin, rho * phi0_.cos * c.cos - y * phi0_.sin * c.sin);
        const auto latr = std::asin(c.cos * phi0_.sin + y * c.sin * phi0_.cos / rho);

        return PointLonLat::make(util::RADIAN_TO_DEGREE * lonr, util::RADIAN_TO_DEGREE * latr);
    }

    return PointLonLat::make(centre_.lon, centre_.lat);
}


Spec* LambertAzimuthalEqualArea::spec() const {
    return new spec::Custom({{"projection", "laea"},
                             {"lon_0", centre_.lon},
                             {"lat_0", centre_.lat},
                             {"lon_first", first_.lon},
                             {"lat_first", first_.lat}});
}


}  // namespace eckit::geo::projection
