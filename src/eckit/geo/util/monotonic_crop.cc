// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::util {


using difference_type = std::make_signed_t<size_t>;


std::pair<difference_type, difference_type> monotonic_crop(const std::vector<double>& values, double min, double max) {
    if (values.empty() || min > max) {
        return {};
    }

    auto b = values.begin();
    auto e = values.end();

    // monotonically increasing
    const auto increasing = values.size() == 1 || values.front() < values.back();
    if (increasing) {
        ASSERT(std::is_sorted(b, e));

        auto lt = [](double a, double b) { return a < b; };

        return {std::distance(b, std::lower_bound(b, e, min, lt)), std::distance(b, std::upper_bound(b, e, max, lt))};
    }


    // monotonically non-increasing
    ASSERT(std::is_sorted(values.rbegin(), values.rend()));

    auto gt = [](double a, double b) { return a > b; };

    return {std::distance(b, std::lower_bound(b, e, max, gt)), std::distance(b, std::upper_bound(b, e, min, gt))};
}


}  // namespace eckit::geo::util
