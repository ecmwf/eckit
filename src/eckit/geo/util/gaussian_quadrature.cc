// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "eckit/cache/MemoryCache.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/util.h"


namespace eckit::geo::util {


namespace {


/// Gaussian quadrature of Gaussian number N, of the northern hemisphere: colatitudes [rad] (increasing) of the roots
/// of the Legendre polynomial of degree 2 N, and their weights (the southern hemisphere is symmetric)
struct GaussianQuadrature {
    explicit GaussianQuadrature(size_t N) : colatitudes(N), weights(N) {
        ASSERT(N > 0);

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
                    zfn[i - j] = zfn[i - j + 2] * static_cast<double>((j - 1) * (2 * i - j + 2)) /
                                 static_cast<double>(j * (2 * i - j + 1));
                }
            }

            for (size_t i = 0; i <= N; ++i) {
                zzfn[i] = zfn[i * 2];
            }
        }


        /*
         * Newton loop (per latitude) to find 0 of Legendre polynomial of degree N (GAWL)
         * The convergence is run more than once, for historical reasons (the weight is of this last iteration).
         */
        constexpr size_t Nmax = 20;
        constexpr auto eps    = std::numeric_limits<double>::epsilon() * 1000.;

        for (size_t i = 0; i < N; ++i) {
            // First guess for colatitude [rad]
            auto z = static_cast<double>(4 * (i + 1) - 1) * M_PI / static_cast<double>(4 * 2 * N + 2);
            auto x = z + 1. / (std::tan(z) * static_cast<double>(8 * (2 * N) * (2 * N)));

            auto converged = false;
            auto done      = false;

            // (Nmax iterations to converge, and the one more)
            for (size_t n = 0; n <= Nmax; ++n) {
                auto f = zzfn[0] * 0.5 +
                         zzfn[1] * std::cos(2. * x);  // normalised ordinary Legendre polynomial == \overbar{P_n}^0
                auto fp = zzfn[1] * std::sin(2. * x) * -2.;  // normalised derivative == d/d\theta(\overbar{P_n}^0)

                for (size_t j = 2; j <= N; ++j) {
                    const auto j2 = static_cast<double>(j * 2);
                    f += zzfn[j] * std::cos(j2 * x);
                    fp -= zzfn[j] * std::sin(j2 * x) * j2;
                }

                auto dx = -f / fp;
                x += dx;

                if (converged) {
                    weights[i] = static_cast<double>(2 * 2 * N + 1) / (fp * fp);
                    done       = true;
                    break;
                }

                converged = std::abs(dx) <= eps;
            }

            if (!done) {
                throw exception::RangeError("Gaussian quadrature: N=" + std::to_string(N) + " not converging (within " +
                                                std::to_string(Nmax) + " iterations)",
                                            Here());
            }

            colatitudes[i] = x;
        }
    }

    std::vector<double> colatitudes;
    std::vector<double> weights;
};


}  // namespace


const std::vector<double>& gaussian_latitudes(size_t N, bool increasing) {
    ASSERT(N > 0);

    using cache_t = eckit::cache::MemoryCacheT<std::pair<size_t, bool>, std::vector<double>>;
    const cache_t::key_type key{N, increasing};

    static cache_t cache(LibEcKitGeo::memoryCaches());
    if (const auto* cached = cache.find(key)) {
        return *cached;
    }

    const GaussianQuadrature quadrature(N);

    // colatitude [rad] to latitude [degree], symmetric
    std::vector<double> lats(2 * N);
    for (size_t i = 0; i < N; ++i) {
        const auto x = quadrature.colatitudes[i];
        const auto j = 2 * N - 1 - i;
        lats[i]      = (increasing ? (x - M_PI_2) : (M_PI_2 - x)) * RADIAN_TO_DEGREE;
        lats[j]      = -lats[i];
    }

    return cache.insert(key, std::move(lats));
}


const std::vector<double>& gaussian_quadrature_weights(size_t N) {
    ASSERT(N > 0);

    using cache_t = eckit::cache::MemoryCacheT<size_t, std::vector<double>>;
    const cache_t::key_type key{N};

    static cache_t cache(LibEcKitGeo::memoryCaches());
    if (const auto* cached = cache.find(key)) {
        return *cached;
    }

    const GaussianQuadrature quadrature(N);

    // symmetric
    std::vector<double> weights(2 * N);
    for (size_t i = 0; i < N; ++i) {
        weights[i] = weights[2 * N - 1 - i] = quadrature.weights[i];
    }

    return cache.insert(key, std::move(weights));
}


}  // namespace eckit::geo::util
