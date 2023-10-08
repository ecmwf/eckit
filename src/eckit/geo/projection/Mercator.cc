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


#include "eckit/geo/projection/Mercator.h"

#include <cmath>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<Mercator> __projection("mercator");


Mercator::Mercator(double meridian, double parallel, Figure* figure, PointLonLat first) :
    meridian_(PointLonLat::normalise_angle_to_minimum(meridian, -180.)),
    parallel_(parallel),
    figure_(figure),
    first_(first),
    eps_(1e-10),
    max_iter_(15) {
    // Map Projections: A Working Manual, John P. Snyder (1987)
    // - Equation (7-9) to calculate phi iteratively
    // - Equation (15-11) to calculate t
    ASSERT(figure_);

    ASSERT_MSG(!types::is_approximately_equal(first.lat, 90.) && !types::is_approximately_equal(first.lat, -90.),
               "Mercator: projection cannot be calculated at the poles");

    e_    = figure_->eccentricity();
    lam0_ = util::degree_to_radian * meridian_;

    auto phi0 = util::degree_to_radian * parallel_;
    auto lam1 = util::degree_to_radian * first.lon;
    auto phi1 = util::degree_to_radian * first.lat;

    m_ = figure_->a() * std::cos(phi0) / (std::sqrt(1. - e_ * e_ * std::sin(phi0) * std::sin(phi0)));
    ASSERT(!types::is_approximately_equal(m_, 0.));

    w_  = 1. / m_;
    x0_ = m_ * (lam0_ - lam1);
    y0_ = m_ * std::log(std::tan(M_PI_4 - 0.5 * phi1) /
                        std::pow(((1. - e_ * std::sin(phi1)) / (1. + e_ * std::sin(phi1))), 0.5 * e_));

    ASSERT(types::is_approximately_equal(phi1, calculate_phi(std::exp(y0_ * w_)), eps_));
}


Mercator::Mercator(const Configuration& config) :
    Mercator(config.getDouble("meridian"),
             config.getDouble("parallel"),
             FigureFactory::instance().get(config.getString("figure")).create(config),
             PointLonLat{config.getDouble("lon0"), config.getDouble("lat0")}) {}


double Mercator::calculate_phi(double t) const {
    auto phi = M_PI_2 - 2 * std::atan(t);
    for (size_t i = 0; i <= max_iter_; i++) {
        auto es   = e_ * std::sin(phi);
        auto dphi = M_PI_2 - 2 * std::atan(t * (std::pow(((1. - es * es) / (1. + es * es)), 0.5 * e_))) - phi;

        phi += dphi;
        if (types::is_approximately_equal(dphi, 0., eps_)) {
            return phi;
        }
    }

    throw SeriousBug("Mercator: phi calculation failed to converge");
}


Point2 Mercator::fwd(const PointLonLat& p) const {
    auto phi = util::degree_to_radian * p.lat;
    auto lam = util::degree_to_radian * p.lon;

    return {x0_ + m_ * (lam - lam0_),
            y0_ - m_ * std::log(std::tan(M_PI_4 - 0.5 * phi) /
                                std::pow(((1. - e_ * std::sin(phi)) / (1. + e_ * std::sin(phi))), 0.5 * e_))};
}


PointLonLat Mercator::inv(const Point2& q) const {
    return PointLonLat::make(util::radian_to_degree * (lam0_ + (q.X - x0_) * w_),
                             util::radian_to_degree * calculate_phi(std::exp(-(q.Y - y0_) * w_)));
}


Projection::Spec Mercator::spec() const {
    return Spec{{{"lad", parallel_}, {"orientation", meridian_}}};
}


}  // namespace eckit::geo::projection
