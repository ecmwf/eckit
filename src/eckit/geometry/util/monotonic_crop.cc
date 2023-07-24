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
#include <utility>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geometry::util {


std::pair<std::vector<double>::const_iterator, std::vector<double>::const_iterator> monotonic_crop(
    const std::vector<double>& values, double min, double max, double eps) {
    if (values.empty() || min > max) {
        return {};
    }


    // monotonically increasing
    const auto increasing = values.size() == 1 || values.front() < values.back();
    if (increasing) {
        ASSERT(std::is_sorted(values.begin(), values.end()));

        auto lt = [eps](double a, double b) { return a < b && (0. == eps || !types::is_approximately_equal(a, b, eps)); };
        return {std::lower_bound(values.begin(), values.end(), min, lt),
                std::upper_bound(values.begin(), values.end(), max, lt)};
    }


    // monotonically non-increasing
    ASSERT(std::is_sorted(values.rbegin(), values.rend()));

    auto gt = [eps](double a, double b) { return a > b && (0. == eps || !types::is_approximately_equal(a, b, eps)); };
    return {std::lower_bound(values.begin(), values.end(), max, gt),
            std::upper_bound(values.begin(), values.end(), min, gt)};
}


}  // namespace eckit::geometry::util