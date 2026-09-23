// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/util.h"


namespace eckit::geo::util {


const pl_type& reduced_octahedral_pl(size_t N) {
    static cache::MemoryCacheT<size_t, pl_type> cache;
    if (cache.contains(N)) {
        return cache[N];
    }

    pl_type pl(N * 2);

    pl_type::value_type p = 20;
    for (size_t i = 0, j = 2 * N - 1; i < N; ++i, --j) {
        pl[i] = pl[j] = p;
        p += 4;
    }

    return (cache[N] = std::move(pl));
}


}  // namespace eckit::geo::util
