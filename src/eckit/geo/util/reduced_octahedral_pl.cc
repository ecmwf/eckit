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


#include "eckit/geo/Cache.h"
#include "eckit/geo/util.h"


namespace eckit::geo::util {


const pl_type& reduced_octahedral_pl(size_t N) {
    static CacheT<size_t, pl_type> cache;
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
