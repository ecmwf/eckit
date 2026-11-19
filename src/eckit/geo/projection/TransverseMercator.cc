// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/TransverseMercator.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const ProjectionRegisterType<TransverseMercator> PROJECTION_1("tmerc");
static const ProjectionRegisterType<TransverseMercator> PROJECTION_2("transverse_mercator");


TransverseMercator::TransverseMercator(double lon_0, double lat_0, double k_0, Figure* fig) :
    Projection(fig), lon_0_(lon_0), lat_0_(lat_0), k_0_(k_0), conformal_(figure().conformal()) {
    if (!(std::abs(lat_0_) <= PointLonLat::RIGHT_ANGLE)) {
        throw exception::ProjectionError("TransverseMercator: {lat_0: ...} in [-90, 90] expected", Here());
    }

    if (!(k_0_ > 0.) || !std::isfinite(k_0_)) {
        throw exception::ProjectionError("TransverseMercator: {k_0: ...} > 0 expected", Here());
    }

    // Karney (2011), eqs. (14), (35) and (36), in the third flattening n
    const auto a  = figure().a();
    const auto b  = figure().b();
    const auto n  = (a - b) / (a + b);
    const auto n2 = n * n;
    const auto n3 = n2 * n;
    const auto n4 = n3 * n;
    const auto n5 = n4 * n;
    const auto n6 = n5 * n;

    kA_ = k_0_ * a / (1. + n) * (1. + n2 / 4. + n4 / 64. + n6 / 256.);

    alpha_ = {
        n / 2. - 2. * n2 / 3. + 5. * n3 / 16. + 41. * n4 / 180. - 127. * n5 / 288. + 7891. * n6 / 37800.,
        13. * n2 / 48. - 3. * n3 / 5. + 557. * n4 / 1440. + 281. * n5 / 630. - 1983433. * n6 / 1935360.,
        61. * n3 / 240. - 103. * n4 / 140. + 15061. * n5 / 26880. + 167603. * n6 / 181440.,
        49561. * n4 / 161280. - 179. * n5 / 168. + 6601661. * n6 / 7257600.,
        34729. * n5 / 80640. - 3418889. * n6 / 1995840.,
        212378941. * n6 / 319334400.,
    };

    beta_ = {
        n / 2. - 2. * n2 / 3. + 37. * n3 / 96. - n4 / 360. - 81. * n5 / 512. + 96199. * n6 / 604800.,
        n2 / 48. + n3 / 15. - 437. * n4 / 1440. + 46. * n5 / 105. - 1118711. * n6 / 3870720.,
        17. * n3 / 480. - 37. * n4 / 840. - 209. * n5 / 4480. + 5569. * n6 / 90720.,
        4397. * n4 / 161280. - 11. * n5 / 504. - 830251. * n6 / 7257600.,
        4583. * n5 / 161280. - 108847. * n6 / 3991680.,
        20648693. * n6 / 638668800.,
    };

    // on the central meridian, the conformal latitude is xi'
    const auto chi_0 = M_PI_2 - 2. * std::atan(conformal_.t(lat_0_ * util::DEGREE_TO_RADIAN));
    y_0_             = kA_ * krueger(chi_0, 0., alpha_, 1.).first;
}


TransverseMercator::TransverseMercator(const Spec& spec) :
    TransverseMercator(spec.get_double("lon_0", 0.), spec.get_double("lat_0", 0.), spec.get_double("k_0", 1.),
                       FigureFactory::build(spec)) {}


std::pair<double, double> TransverseMercator::krueger(double xi, double eta, const Series& c, double sign) {
    auto x = xi;
    auto y = eta;
    for (size_t j = 1; j <= c.size(); ++j) {
        const auto j2 = 2. * static_cast<double>(j);
        x += sign * c[j - 1] * std::sin(j2 * xi) * std::cosh(j2 * eta);
        y += sign * c[j - 1] * std::cos(j2 * xi) * std::sinh(j2 * eta);
    }
    return {x, y};
}


PointXY TransverseMercator::fwd(const PointLonLat& p) const {
    const auto lam =
        PointLonLat::normalise_angle_to_minimum(p.lon() - lon_0_, -PointLonLat::FLAT_ANGLE) * util::DEGREE_TO_RADIAN;
    const auto chi = M_PI_2 - 2. * std::atan(conformal_.t(p.lat() * util::DEGREE_TO_RADIAN));

    // Gauss-Schreiber (xi', eta') on the conformal sphere, Karney (2011) eq. (10), then on the ellipsoid
    const auto [xi, eta] = krueger(std::atan2(std::sin(chi), std::cos(chi) * std::cos(lam)),
                                   std::atanh(std::cos(chi) * std::sin(lam)), alpha_, 1.);

    return {kA_ * eta, kA_ * xi - y_0_};
}


PointLonLat TransverseMercator::inv(const PointXY& q) const {
    const auto [xi, eta] = krueger((q.Y() + y_0_) / kA_, q.X() / kA_, beta_, -1.);

    // Karney (2011) eq. (18), and the conformal latitude to the latitude
    const auto chi = std::atan2(std::sin(xi), std::hypot(std::sinh(eta), std::cos(xi)));
    const auto lam = std::atan2(std::sinh(eta), std::cos(xi));

    return PointLonLat::make_from_lonlatr(lon_0_ * util::DEGREE_TO_RADIAN + lam,
                                          conformal_.phi(std::tan(M_PI_4 - chi / 2.)),
                                          lon_0_ - PointLonLat::FLAT_ANGLE);
}


const std::string& TransverseMercator::type() const {
    static const std::string type{"tmerc"};
    return type;
}


void TransverseMercator::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", type());

    if (!types::is_approximately_equal(k_0_, 1.)) {
        custom.set("k_0", k_0_);
    }

    if (!types::is_approximately_equal(lat_0_, 0.)) {
        custom.set("lat_0", lat_0_);
    }

    if (!types::is_approximately_equal(lon_0_, 0.)) {
        custom.set("lon_0", lon_0_);
    }
}


}  // namespace eckit::geo::projection
