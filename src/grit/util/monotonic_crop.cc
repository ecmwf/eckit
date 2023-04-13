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
#include <iostream>
#include <iterator>

#include "grit/exception.h"
#include "grit/util.h"


namespace grit::util {


std::vector<double> monotonic_crop(const std::vector<double>& values, double min, double max, double eps) {
    if (values.empty() || min > max) {
        return {};
    }

    ASSERT(!values.empty() && min <= max);

    auto lt = [eps](double a, double b) { return a < b && (0. == eps || !is_approximately_equal(a, b, eps)); };

    if (values.size() == 1 || values.front() < values.back()) {
        // monotonically increasing
        ASSERT(std::is_sorted(values.begin(), values.end()));

        return {std::lower_bound(values.begin(), values.end(), min, lt),
                std::upper_bound(values.begin(), values.end(), max, lt)};
    }

    // monotonically decreasing
    ASSERT(std::is_sorted(values.rbegin(), values.rend()));

    return {};  // FIXME
}


}  // namespace grit::util
