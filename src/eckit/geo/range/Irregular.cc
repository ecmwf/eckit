// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/range/Irregular.h"

#include <algorithm>
#include <iterator>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::range {


Irregular::Irregular(const std::vector<double>& values) : values_(values) {
    if (values_.empty()) {
        throw exception::RangeError("Irregular: values expected (not empty)", Here());
    }

    const auto increasing = this->increasing();
    for (size_t i = 1; i < values_.size(); ++i) {
        if (increasing ? !(values_[i - 1] < values_[i]) : !(values_[i - 1] > values_[i])) {
            throw exception::RangeError("Irregular: values expected strictly monotonic", Here());
        }
    }
}


Irregular* Irregular::make_cropped_range(double crop_a, double crop_b) const {
    const auto lo = std::min(crop_a, crop_b);
    const auto hi = std::max(crop_a, crop_b);

    std::vector<double> values;
    std::copy_if(values_.begin(), values_.end(), std::back_inserter(values),
                 [lo, hi](double v) { return lo <= v && v <= hi; });

    if (values.empty()) {
        throw exception::RangeError("Irregular: cropping results in an empty range", Here());
    }

    return new Irregular(values);
}


}  // namespace eckit::geo::range
