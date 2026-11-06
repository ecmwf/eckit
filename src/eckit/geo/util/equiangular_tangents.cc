// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <cstddef>
#include <vector>

#include "eckit/cache/MemoryCache.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/util.h"


namespace eckit::geo::util {


const std::vector<double>& equiangular_tangents(size_t n) {
    ASSERT(n > 0);

    static eckit::cache::MemoryCacheT<size_t, std::vector<double>> cache(LibEcKitGeo::memoryCaches());
    if (const auto* cached = cache.find(n)) {
        return *cached;
    }

    // antisymmetric by construction, so nodes shared by panels are bitwise equal
    const auto step = M_PI_2 / static_cast<double>(n);

    std::vector<double> t(n + 1);
    for (size_t k = 0, l = n; k < l; ++k, --l) {
        t[k] = std::tan(-M_PI_4 + static_cast<double>(k) * step);
        t[l] = -t[k];
    }

    if (n % 2 == 0) {
        t[n / 2] = 0.;
    }

    t.front() = -1.;
    t.back()  = 1.;

    return cache.insert(n, std::move(t));
}


}  // namespace eckit::geo::util
