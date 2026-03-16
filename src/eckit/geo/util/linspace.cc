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
