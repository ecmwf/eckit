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


#include "eckit/geo/range/GaussianLatitude.h"

#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


static util::recursive_mutex MUTEX;


GaussianLatitude::GaussianLatitude(size_t N, bool increasing, double eps) :
    Range(2 * N, increasing ? -90. : 90., increasing ? 90. : -90., eps), N_(N) {}


Range* GaussianLatitude::make_range_flipped() const {
    std::vector<double> flipped(size());
    const auto& v = values();
    std::reverse_copy(v.begin(), v.end(), flipped.begin());

    return new GaussianLatitude(N_, std::move(flipped), eps());
}


Range* GaussianLatitude::make_range_cropped(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b)
           || (types::is_approximately_equal(a(), b(), eps()) && types::is_approximately_equal(crop_a, crop_b, eps())));

    auto v = values();

    if ((a() < b()) && (a() < crop_a || crop_b < b())) {
        auto [from, to] = util::monotonic_crop(v, crop_a, crop_b, eps());
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);
    }
    else if ((b() < a()) && (b() < crop_b || crop_a < a())) {
        auto [from, to] = util::monotonic_crop(v, crop_b, crop_a, eps());
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);
    }

    return new GaussianLatitude(N_, std::move(v), eps());
}


Fraction GaussianLatitude::increment() const {
    NOTIMP;
}


const std::vector<double>& GaussianLatitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    return values_.empty() ? util::gaussian_latitudes(N_, a() < b()) : values_;
}


}  // namespace eckit::geo::range
