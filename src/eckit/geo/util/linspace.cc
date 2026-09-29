// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <vector>

#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"


namespace eckit::geo::util {


const std::vector<double>& linspace(double start, double stop, size_t num) {
    const auto key = (eckit::MD5{} << start << stop << num).digest();

    static cache::MemoryCacheT<decltype(key), std::vector<double>> cache;
    if (cache.contains(key)) {
        return cache[key];
    }

    if (num == 0) {
        return (cache[key] = {});
    }

    if (num == 1 || types::is_approximately_equal(start, stop)) {
        return (cache[key] = {start});
    }

    const auto step = (stop - start) / static_cast<double>(num - 1);

    std::vector<double> l(num);
    std::generate_n(l.begin(), num,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return (cache[key] = std::move(l));
}


}  // namespace eckit::geo::util
