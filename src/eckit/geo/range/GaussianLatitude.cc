// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/range/GaussianLatitude.h"

#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


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

    if (is_equal(a(), b())) {
        ASSERT(is_equal(crop_a, crop_b));
    }
    else if (a() < b()) {
        ASSERT(crop_a <= crop_b);
        if (is_less(a(), crop_a) || is_less(crop_b, b())) {
            auto [from, to] = util::monotonic_crop(v, crop_a, crop_b);
            v.erase(v.begin() + to, v.end());
            v.erase(v.begin(), v.begin() + from);
        }
    }
    else {
        ASSERT(crop_a >= crop_b);
        if (is_less(b(), crop_b) || is_less(crop_a, a())) {
            auto [from, to] = util::monotonic_crop(v, crop_b, crop_a);
            v.erase(v.begin() + to, v.end());
            v.erase(v.begin(), v.begin() + from);
        }
    }

    return new GaussianLatitude(N_, std::move(v));
}


const std::vector<double>& GaussianLatitude::values() const {
    lock_type lock;

    return values_.empty() ? util::gaussian_latitudes(N_, a() < b()) : values_;
}


}  // namespace eckit::geo::range
