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
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::util {


using difference_type = std::make_signed_t<size_t>;


std::pair<difference_type, difference_type> monotonic_crop(const std::vector<double>& values, double min, double max,
                                                           double eps) {
    if (values.empty() || min > max) {
        return {};
    }

    auto b = values.begin();
    auto e = values.end();

    // monotonically increasing
    const auto increasing = values.size() == 1 || values.front() < values.back();
    if (increasing) {
        ASSERT(std::is_sorted(b, e));

        auto lt
            = [eps](double a, double b) { return a < b && (0. == eps || !types::is_approximately_equal(a, b, eps)); };

        return {std::distance(b, std::lower_bound(b, e, min, lt)), std::distance(b, std::upper_bound(b, e, max, lt))};
    }


    // monotonically non-increasing
    ASSERT(std::is_sorted(values.rbegin(), values.rend()));

    auto gt = [eps](double a, double b) { return a > b && (0. == eps || !types::is_approximately_equal(a, b, eps)); };

    return {std::distance(b, std::lower_bound(b, e, max, gt)), std::distance(b, std::upper_bound(b, e, min, gt))};
}


}  // namespace eckit::geo::util
