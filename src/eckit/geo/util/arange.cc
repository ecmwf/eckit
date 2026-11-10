// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "eckit/types/FloatCompare.h"


namespace eckit::geo::util {


std::vector<double> arange(double start, double stop, double step) {
    if (types::is_approximately_equal(step, 0.) || types::is_approximately_equal(start, stop) ||
        (stop - start) * step < 0.) {
        std::vector<double> l(1, start);
        return l;
    }

    // the number of steps, rounded if an integer within rounding errors (e.g. 0.3 / 0.1), otherwise truncated
    const auto steps   = (stop - start) / step;
    const auto to_stop = types::is_approximately_equal(steps, std::round(steps));

    const auto num = static_cast<size_t>(to_stop ? std::round(steps) : std::floor(steps)) + 1;

    std::vector<double> l(num);
    std::generate_n(l.begin(), num,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    if (to_stop) {
        l.back() = stop;  // no rounding past the end (e.g. a pole)
    }

    return l;
}


}  // namespace eckit::geo::util
