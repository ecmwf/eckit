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
#include <limits>

#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<Mercator> PROJECTION_1("mercator");
static ProjectionBuilder<Mercator> PROJECTION_2("merc");


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
    lam0_ = util::DEGREE_TO_RADIAN * meridian_;

    auto phi0 = util::DEGREE_TO_RADIAN * parallel_;
    auto lam1 = util::DEGREE_TO_RADIAN * first.lon;
    auto phi1 = util::DEGREE_TO_RADIAN * first.lat;

    m_ = figure_->a() * std::cos(phi0) / (std::sqrt(1. - e_ * e_ * std::sin(phi0) * std::sin(phi0)));
    ASSERT(!types::is_approximately_equal(m_, 0.));

    w_  = 1. / m_;
    x0_ = m_ * (lam0_ - lam1);
    y0_ = m_
          * std::log(std::tan(M_PI_4 - 0.5 * phi1)
                     / std::pow(((1. - e_ * std::sin(phi1)) / (1. + e_ * std::sin(phi1))), 0.5 * e_));

    ASSERT(types::is_approximately_equal(phi1, calculate_phi(std::exp(y0_ * w_)), eps_));
}


Mercator::Mercator(const Spec& spec) :
    Mercator(spec.get_double("meridian"), spec.get_double("parallel"),
             FigureFactory::instance().get(spec.get_string("figure")).create(spec),
             PointLonLat{spec.get_double("lon0"), spec.get_double("lat0")}) {}


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

    throw SeriousBug("Mercator: phi calculation failed to converge", Here());
}


Point2 Mercator::fwd(const PointLonLat& p) const {
    auto phi = util::DEGREE_TO_RADIAN * p.lat;
    auto lam = util::DEGREE_TO_RADIAN * p.lon;
    auto s   = std::sin(phi);

    return {
        x0_ + m_ * (lam - lam0_),
        types::is_approximately_equal(s, 1.) ? std::numeric_limits<double>::infinity()
        : types::is_approximately_equal(s, -1.)
            ? -std::numeric_limits<double>::infinity()
            : y0_ - m_ * std::log(std::tan(M_PI_4 - 0.5 * phi) / std::pow(((1. - e_ * s) / (1. + e_ * s)), 0.5 * e_))};
}


PointLonLat Mercator::inv(const Point2& q) const {
    return PointLonLat::make(util::RADIAN_TO_DEGREE * (lam0_ + (q.X - x0_) * w_),
                             util::RADIAN_TO_DEGREE * calculate_phi(std::exp(-(q.Y - y0_) * w_)));
}


Spec* Mercator::spec() const {
    return new spec::Custom{{{"projection", "mercator"}, {"lad", parallel_}, {"orientation", meridian_}}};
}


}  // namespace eckit::geo::projection
