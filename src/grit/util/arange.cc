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
#include <cmath>
#include <limits>

#include "grit/util.h"


namespace grit::util {


template <typename T>
bool approximately_equal(T x, T y, T eps = std::numeric_limits<T>::epsilon()) {
    auto min = std::min(std::abs(x), std::abs(y));
    return std::abs(min) == 0. ? std::abs(x - y) < eps
                               : std::abs(x - y) / std::max(std::numeric_limits<T>::min(), min) < eps;
};


std::vector<double> arange(double start, double stop, double step) {
    if (approximately_equal(step, 0.) || approximately_equal(start, stop) || (stop - start) * step < 0.) {
        std::vector<double> l(1, start);
        return l;
    }

    const auto num = static_cast<size_t>((stop - start) / step) + 1;

    std::vector<double> l(num);
    std::generate_n(l.begin(), num, [start, step, n = 0]() mutable { return start + static_cast<double>(n++) * step; });

    return l;
}


}  // namespace grit::util
