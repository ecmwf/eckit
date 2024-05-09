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


#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Cache.h"
#include "eckit/geo/util.h"


namespace eckit::geo::util {


const std::vector<double>& gaussian_latitudes(size_t N, bool increasing) {
    ASSERT(N > 0);

    using cache_t = CacheT<std::pair<size_t, bool>, std::vector<double>>;
    const cache_t::key_type key{N, increasing};

    static cache_t cache;
    if (cache.contains(key)) {
        return cache[key];
    }

    std::vector<double> lats(2 * N);


    // Fourier coefficients of series expansion for the ordinary Legendre polynomials
    std::vector<double> zzfn(N + 1);
    {
        // Belousov, Swarztrauber use zfn(0)=std::sqrt(2.)
        // IFS normalisation chosen to be 0.5*Integral(Pnm**2) = 1
        std::vector<double> zfn(2 * N + 1, 2.);

        for (size_t i = 1; i <= 2 * N; ++i) {
            for (size_t j = 1; j <= i; ++j) {
                zfn[i] *= std::sqrt(1. - 0.25 / (static_cast<double>(j * j)));
            }

            for (size_t j = 2; j <= i - (i % 2); j += 2) {
                zfn[i - j] = zfn[i - j + 2] * static_cast<double>((j - 1) * (2 * i - j + 2))
                             / static_cast<double>(j * (2 * i - j + 1));
            }
        }

        for (size_t i = 0; i <= N; ++i) {
            zzfn[i] = zfn[i * 2];
        }
    }


    // Newton loop (per latitude) to find 0 of Legendre polynomial of degree N (GAWL)
    constexpr size_t Nmax = 20;
    constexpr auto eps    = std::numeric_limits<double>::epsilon() * 1000.;

    for (size_t i = 0; i < N; ++i) {
        // First guess for colatitude [rad]
        double z = static_cast<double>(4 * (i + 1) - 1) * M_PI / static_cast<double>(4 * 2 * N + 2);
        double x = (z + 1. / (std::tan(z) * static_cast<double>(8 * (2 * N) * (2 * N))));

        auto converged = false;

        for (size_t n = 0; n < Nmax; ++n) {
            auto f  = 0.5 * zzfn[0];  // normalised ordinary Legendre polynomial == \overbar{P_n}^0
            auto fp = 0.;             // normalised derivative == d/d\theta(\overbar{P_n}^0)

            for (size_t i = 1; i <= N; ++i) {
                const auto i2 = static_cast<double>(i * 2);
                f += zzfn[i] * std::cos(i2 * x);
                fp -= zzfn[i] * std::sin(i2 * x) * i2;
            }

            auto dx = -f / fp;
            x += dx;

            if (converged) {
                break;
            }

            converged = std::abs(dx) <= eps;
        }

        if (!converged) {
            throw BadValue("Could not calculate latitude within accuracy/iterations: " + std::to_string(eps) + "/"
                               + std::to_string(Nmax),
                           Here());
        }

        // Convert colatitude [rad] to latitude [degree], symmetry
        const auto j = 2 * N - 1 - i;
        lats[i]      = (increasing ? (x - M_PI_2) : (M_PI_2 - x)) * RADIAN_TO_DEGREE;
        lats[j]      = -lats[i];
    }


    return (cache[key] = std::move(lats));
}


}  // namespace eckit::geo::util
