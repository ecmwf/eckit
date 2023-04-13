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


#include "grit/exception.h"
#include "grit/util.h"


namespace grit::util {


std::vector<double> monotonic_crop(const std::vector<double>& values, double min, double max, double eps) {
    ASSERT(!values.empty());
    ASSERT(min <= max);

    const bool increasing = values.front() <= values.back();

    if (max < values.back()) {
        max = values.back();
    }
    else if (max <= values.front()) {
        auto it = std::lower_bound(values.rbegin(), values.rend(), max);
        max     = *it;
    }

    if (min > values.front()) {
        min = values.front();
    }
    else if (min >= values.back()) {
        auto it = std::lower_bound(values.begin(), values.end(), min, [](double l1, double l2) { return l1 > l2; });
        min     = *it;
    }

    ASSERT(min <= max);

    // FIXME
    return values;
}


}  // namespace grit::util
