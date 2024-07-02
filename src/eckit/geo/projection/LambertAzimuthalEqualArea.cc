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

#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<LambertAzimuthalEqualArea> PROJECTION_1("lambert_azimuthal_equal_area");
static ProjectionBuilder<LambertAzimuthalEqualArea> PROJECTION_2("laea");


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(const Spec& spec) :
    LambertAzimuthalEqualArea({spec.get_double("lon_0"), spec.get_double("lat_0")},
                              {spec.get_double("first_lon"), spec.get_double("first_lat")}) {}


LambertAzimuthalEqualArea::LambertAzimuthalEqualArea(PointLonLat centre, PointLonLat first) :
    centre_(centre),
    centre_r_(PointLonLatR::make_from_lonlat(centre.lon, centre.lat)),
    first_(first),
    first_r_(PointLonLatR::make_from_lonlat(first.lon, first.lat)),
    phi0_(centre_r_.latr),
    phi_(first_r_.latr),
    dlam_(first_r_.lonr - centre_r_.lonr) {}


Point2 LambertAzimuthalEqualArea::fwd(const PointLonLat& p) const {
    const auto kp = figure().R() * std::sqrt(2. / (1. + phi0_.sin * phi_.sin + phi0_.cos * phi_.cos * dlam_.cos));

    auto x = kp * phi_.cos * dlam_.sin;
    auto y = kp * (phi0_.cos * phi_.sin - phi0_.sin * phi_.cos * dlam_.cos);

    return {x, y};
}


PointLonLat LambertAzimuthalEqualArea::inv(const Point2& p) const {
    auto rho = std::sqrt(p.X * p.X + p.Y * p.Y);
    const util::sincos_t c(2. * std::asin(rho / (2. * figure().R())));

    return PointLonLat::make_from_lonlatr(
        centre_r_.lonr + std::atan2(p.X * c.sin, rho * phi0_.cos * c.cos - p.Y * phi0_.sin * c.sin),
        std::asin(c.cos * phi0_.sin + p.Y * c.sin * phi0_.cos / rho));
}


void LambertAzimuthalEqualArea::fill_spec(spec::Custom& custom) const {
    ProjectionOnFigure::fill_spec(custom);

    custom.set("projection", "laea");
    custom.set("lon_0", centre_.lon);
    custom.set("lat_0", centre_.lat);
    custom.set("lon_first", first_.lon);
    custom.set("lat_first", first_.lat);
}


}  // namespace eckit::geo::projection
