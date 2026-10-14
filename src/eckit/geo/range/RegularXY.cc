// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/range/RegularXY.h"

#include <algorithm>
#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


RegularXY::RegularXY(double inc, double a, double b) : inc_(inc) {
    // number of steps from a to b, b included if within a small fraction of the step
    const auto steps = types::is_approximately_equal(inc, 0.) ? 0. : std::floor((b - a) / inc + 1e-6);
    const auto n     = steps > 0. ? static_cast<size_t>(steps) + 1 : 1;

    if (n == 1) {
        inc_ = 0.;
    }

    values_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        values_[i] = a + static_cast<double>(i) * inc_;
    }
}


RegularXY* RegularXY::make_cropped_range(double crop_a, double crop_b) const {
    const auto lo = std::min(crop_a, crop_b);
    const auto hi = std::max(crop_a, crop_b);

    auto inside      = [lo, hi](double v) { return lo <= v && v <= hi; };
    const auto first = std::find_if(values_.begin(), values_.end(), inside);
    const auto last  = std::find_if(values_.rbegin(), values_.rend(), inside);

    if (first == values_.end()) {
        throw exception::RangeError("RegularXY: cropping results in an empty range", Here());
    }

    return new RegularXY(inc_, *first, *last);
}


Fraction RegularXY::increment() const {
    return Fraction{inc_};
}


}  // namespace eckit::geo::range
