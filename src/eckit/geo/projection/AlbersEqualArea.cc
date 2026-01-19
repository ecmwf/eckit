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


#include "eckit/geo/projection/AlbersEqualArea.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<AlbersEqualArea> PROJECTION_1("aea");
static const ProjectionRegisterType<AlbersEqualArea> PROJECTION_2("albers");


static inline double calculate_C(double lat_1, double n) {
    const auto lat_1r = lat_1 * util::DEGREE_TO_RADIAN;

    return std::cos(lat_1r) * std::cos(lat_1r) + 2. * n * std::sin(lat_1r);
}


static inline double calculate_n(double lat_1, double lat_2) {
    if (auto n = 0.5 * (std::sin(lat_1 * util::DEGREE_TO_RADIAN) / std::sin(lat_2 * util::DEGREE_TO_RADIAN));
        !types::is_approximately_equal(n, 0.)) {
        return n;
    }

    throw exception::ProjectionError("AlbersEqualArea: cannot calculate n", Here());
}


static inline double calculate_rho(double R, double n, double C, double phir) {
    return R / n * std::sqrt(C - 2. * n * std::sin(phir));
}


AlbersEqualArea::AlbersEqualArea(const Spec& spec) :
    AlbersEqualArea(spec.get_double("lon_0"), spec.get_double("lat_0"), spec.get_double("lat_1"),
                    spec.get_double("lat_2"), FigureFactory::build(spec)) {}


AlbersEqualArea::AlbersEqualArea(double lon_0, double lat_0, double lat_1, double lat_2, Figure* _figure) :
    Projection(_figure),
    centre_(lon_0, lat_0),
    centre_r_(PointLonLatR::make_from_lonlat(lon_0, lat_0)),
    lat_1_(lat_1),
    lat_2_(lat_2),
    n_(calculate_n(lat_1, lat_2)),
    R_(figure().R()),
    C_(calculate_C(lat_1, n_)),
    rho0_(calculate_rho(R_, n_, C_, centre_r_.latr)) {}


PointXY AlbersEqualArea::fwd(const PointLonLat& p) const {
    auto pr     = PointLonLatR::make_from_lonlat(p.lon, p.lat);
    auto rho    = calculate_rho(R_, n_, C_, pr.latr);
    auto thetar = n_ * (pr.lonr - centre_r_.lonr);

    return PointXY{rho * std::sin(thetar), rho0_ - rho * std::cos(thetar)} + falseXY();
}


PointLonLat AlbersEqualArea::inv(const PointXY& p) const {
    auto q = p - falseXY();

    auto rho    = std::sqrt(q.X * q.X + (rho0_ - q.Y) * (rho0_ - q.Y));
    auto thetar = std::atan2(q.X, rho0_ - q.Y);

    return PointLonLat::make_from_lonlatr(centre_r_.lonr + thetar / n_,
                                          std::asin((C_ - (rho * rho * n_ * n_) / (R_ * R_)) / (2. * n_)),
                                          centre_.lon - PointLonLat::FLAT_ANGLE);
}


const std::string& AlbersEqualArea::type() const {
    static const std::string type{"aea"};
    return type;
}


void AlbersEqualArea::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());
    custom.set("lon_0", centre_.lon);
    custom.set("lat_0", centre_.lat);
    custom.set("lat_1", lat_1_);
    custom.set("lat_2", lat_2_);
}


}  // namespace eckit::geo::projection
