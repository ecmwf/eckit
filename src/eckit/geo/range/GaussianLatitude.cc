// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/range/GaussianLatitude.h"

#include <algorithm>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


inline bool is_equal(double a, double b) {
    return types::is_approximately_equal(a, b, PointLonLat::EPS);
}


inline bool is_less(double a, double b) {
    return a < b && !is_equal(a, b);
}


GaussianLatitude::GaussianLatitude(size_t N, bool increasing) :
    GaussianLatitude(N, std::vector<double>(util::gaussian_latitudes(N, increasing))) {}


bool GaussianLatitude::includesNorthPole() const {
    const auto increasing = a() < b();
    const auto& lats(util::gaussian_latitudes(N_, increasing));
    return increasing ? is_equal(b(), lats.back()) : is_equal(a(), lats.front());
}


bool GaussianLatitude::includesSouthPole() const {
    const auto increasing = a() < b();
    const auto& lats(util::gaussian_latitudes(N_, increasing));
    return !increasing ? is_equal(b(), lats.back()) : is_equal(a(), lats.front());
}


GaussianLatitude* GaussianLatitude::make_cropped_range(double crop_a, double crop_b) const {
    auto v = values();

    if (v.size() <= 1) {
        // a single value (or none), as is if within the crop
        const auto [lo, hi] = std::minmax(crop_a, crop_b);
        if (!v.empty() && (is_less(v.front(), lo) || is_less(hi, v.front()))) {
            v.clear();
        }
    }
    else if (a() < b()) {
        ASSERT(crop_a <= crop_b);
        if (is_less(a(), crop_a) || is_less(crop_b, b())) {
            auto [from, to] = util::monotonic_crop(v, crop_a - PointLonLat::EPS, crop_b + PointLonLat::EPS);
            v.erase(v.begin() + to, v.end());
            v.erase(v.begin(), v.begin() + from);
        }
    }
    else {
        ASSERT(crop_a >= crop_b);
        if (is_less(b(), crop_b) || is_less(crop_a, a())) {
            auto [from, to] = util::monotonic_crop(v, crop_b - PointLonLat::EPS, crop_a + PointLonLat::EPS);
            v.erase(v.begin() + to, v.end());
            v.erase(v.begin(), v.begin() + from);
        }
    }

    return new GaussianLatitude(N_, std::move(v));
}


const std::vector<double>& GaussianLatitude::values() const {
    return values_;
}


}  // namespace eckit::geo::range
