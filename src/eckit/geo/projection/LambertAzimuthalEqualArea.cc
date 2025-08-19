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

#include "eckit/geo/Figure.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<LambertAzimuthalEqualArea> PROJECTION_1("laea");
static ProjectionRegisterType<LambertAzimuthalEqualArea> PROJECTION_2("lambert_azimuthal_equal_area");


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Spec& spec) : LambertAzimuthalEqualArea({}, {}) {
    // NOTIMP;
}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(PointLonLat centre, PointLonLat first) :
    centre_(centre),
    centre_r_(PointLonLatR::make_from_lonlat(centre.lon, centre.lat)),
    first_(first),
    first_r_(PointLonLatR::make_from_lonlat(first.lon, first.lat)),
    phi0_(centre_r_.latr),
    phi_(first_r_.latr),
    dlam_(first_r_.lonr - centre_r_.lonr) {}


PointXY LambertAzimuthalEqualArea::fwd(const PointLonLat& p) const {
    const auto kp = figure().R() * std::sqrt(2. / (1. + phi0_.sin * phi_.sin + phi0_.cos * phi_.cos * dlam_.cos));

    auto x = kp * phi_.cos * dlam_.sin;
    auto y = kp * (phi0_.cos * phi_.sin - phi0_.sin * phi_.cos * dlam_.cos);

    return {x, y};
}


PointLonLat LambertAzimuthalEqualArea::inv(const PointXY& p) const {
    auto rho = std::sqrt(p.X * p.X + p.Y * p.Y);
    const util::sincos_t c(2. * std::asin(rho / (2. * figure().R())));

    return PointLonLat::make_from_lonlatr(
        centre_r_.lonr + std::atan2(p.X * c.sin, rho * phi0_.cos * c.cos - p.Y * phi0_.sin * c.sin),
        std::asin(c.cos * phi0_.sin + p.Y * c.sin * phi0_.cos / rho), centre_.lon - PointLonLat::FLAT_ANGLE);
}


const std::string& LambertAzimuthalEqualArea::type() const {
    static const std::string type{"laea"};
    return type;
}


void LambertAzimuthalEqualArea::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());
    custom.set("centre_lonlat", std::vector<double>{centre_.lon, centre_.lat});
    custom.set("first_lonlat", std::vector<double>{first_.lon, first_.lat});
}


}  // namespace eckit::geo::projection
