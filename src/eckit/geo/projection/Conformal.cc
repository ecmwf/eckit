// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/Conformal.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::projection {


double Conformal::m(double phi) const {
    const auto s = e_ * std::sin(phi);
    return std::cos(phi) / std::sqrt(1. - s * s);
}


double Conformal::t(double phi) const {
    const auto s = e_ * std::sin(phi);
    return std::tan(M_PI_4 - phi / 2.) / std::pow((1. - s) / (1. + s), e_ / 2.);
}


double Conformal::phi(double t) const {
    constexpr int max_iter = 15;
    constexpr double eps   = 1e-12;

    auto phi = M_PI_2 - 2. * std::atan(t);
    for (int i = 0; i < max_iter; ++i) {
        const auto s    = e_ * std::sin(phi);
        const auto next = M_PI_2 - 2. * std::atan(t * std::pow((1. - s) / (1. + s), e_ / 2.));
        if (std::abs(next - phi) < eps) {
            return next;
        }
        phi = next;
    }

    throw exception::ProjectionError("Conformal: latitude calculation failed to converge", Here());
}


}  // namespace eckit::geo::projection
