// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "eckit/cache/MemoryCache.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {


const std::vector<double>& linspace(double start, double stop, size_t num) {
    const auto key = (eckit::MD5{} << start << stop << num).digest();

    static eckit::cache::MemoryCacheT<decltype(key), std::vector<double>> cache(LibEcKitGeo::memoryCaches());
    if (const auto* cached = cache.find(key)) {
        return *cached;
    }

    if (num == 0) {
        return cache.insert(key, {});
    }

    if (num == 1 || types::is_approximately_equal(start, stop)) {
        return cache.insert(key, {start});
    }

    const auto step = (stop - start) / static_cast<double>(num - 1);

    std::vector<double> l(num);
    std::generate_n(l.begin(), num,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });
    l.back() = stop;  // no rounding past the end (e.g. a pole)

    return cache.insert(key, std::move(l));
}


}  // namespace eckit::geo::util
