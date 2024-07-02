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


#include "eckit/geo/projection/PolarStereographic.h"

#include <cmath>

#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


PolarStereographic::PolarStereographic(const Spec& spec) :
    PolarStereographic({spec.get_double("lon_0"), spec.get_double("lat_0")},
                       {spec.get_double("first_lon"), spec.get_double("first_lat")}) {}


PolarStereographic::PolarStereographic(PointLonLat centre, PointLonLat first, Figure* figure_ptr) :
    ProjectionOnFigure(figure_ptr),
    centre_(PointLonLat::make(centre.lon, centre.lat)),
    centre_r_(PointLonLatR::make_from_lonlat(centre.lon, centre.lat)),
    first_(first),
    first_r_(PointLonLatR::make_from_lonlat(first.lon, first.lat)),
    sign_(centre_.lat < 0. ? -1. : 1.),
    F_(types::is_approximately_equal(centre_.lat, PointLonLat::RIGHT_ANGLE, PointLonLat::EPS)
               || types::is_approximately_equal(centre_.lat, -PointLonLat::RIGHT_ANGLE, PointLonLat::EPS)
           ? 0.5
           : std::tan(0.5 * (M_PI_2 - sign_ * centre_r_.latr)) / std::cos(sign_ * centre_r_.latr)) {
    auto z = fwd(first_);
    x0_    = z.X;
    y0_    = z.Y;
}


Point2 PolarStereographic::fwd(const PointLonLat& q) const {
    auto p = PointLonLatR::make_from_lonlat(q.lon, q.lat);

    auto a      = sign_ * (p.lonr - centre_r_.lonr);
    auto tsf    = std::tan(0.5 * (M_PI_2 - sign_ * p.latr));
    auto height = figure().R() * tsf / F_;

    return {-sign_ * height * std::sin(a), sign_ * height * std::cos(a)};
}


PointLonLat PolarStereographic::inv(const Point2& q) const {
    Point2 p{q.X - x0_, q.Y - y0_};

    auto rh  = std::sqrt(p.X * p.X + p.Y * p.Y);
    auto tsi = rh / figure().R() * F_;

    return PointLonLat::make_from_lonlatr(sign_ * std::atan2(sign_ * p.X, -sign_ * p.Y) + centre_r_.lonr,
                                          sign_ * (M_PI_2 - 2 * std::atan(tsi)));
}


void PolarStereographic::fill_spec(spec::Custom& custom) const {
    ProjectionOnFigure::fill_spec(custom);

    custom.set("projection", "stere");
    custom.set("lon_0", centre_.lon);
    custom.set("lat_0", centre_.lat);
    custom.set("lon_first", first_.lon);
    custom.set("lat_first", first_.lat);
}


}  // namespace eckit::geo::projection
