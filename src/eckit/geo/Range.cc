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


#include "eckit/geo/Range.h"

#include <algorithm>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo {


Range::Range(size_t n, double _a, double _b, double _eps) :
    n_(n), a_(_a), b_(_b), eps_(_eps) {
    ASSERT(n > 0);
    ASSERT(eps_ >= 0);
}


void Range::resize(size_t n) {
    ASSERT(n > 0);
    n_ = n;
}


namespace range {


namespace {


util::recursive_mutex MUTEX;

constexpr auto DB = 1e-12;


Fraction regular_adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
};


}  // namespace


GaussianLatitude::GaussianLatitude(size_t N, bool increasing, double eps) :
    Range(2 * N, increasing ? -90. : 90., increasing ? 90. : -90., eps), N_(N) {}


Range* GaussianLatitude::flip() const {
    std::vector<double> flipped(size());
    const auto& v = values();
    std::reverse_copy(v.begin(), v.end(), flipped.begin());

    return new GaussianLatitude(N_, std::move(flipped), eps());
}


Range* GaussianLatitude::crop(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b) ||
           (types::is_approximately_equal(a(), b(), eps()) && types::is_approximately_equal(crop_a, crop_b, eps())));

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


const std::vector<double>& GaussianLatitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);
    return values_.empty() ? util::gaussian_latitudes(N_, a() < b()) : values_;
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double _eps) :
    Range(n, _a, types::is_approximately_equal(_a, _b, _eps) ? _a : _b, _eps) {
    if (types::is_approximately_equal(a(), b(), eps())) {
        resize(1);
        periodic_ = NonPeriodic;
        values_   = {a()};
    }
    else if (a() < b()) {
        auto new_b = PointLonLat::normalise_angle_to_minimum(b() - DB, a()) + DB;
        periodic_  = types::is_approximately_lesser_or_equal(360., (new_b - a()) * (1. + 1. / static_cast<double>(n)))
                         ? PeriodicNoEndPoint
                         : NonPeriodic;
        b(periodic() ? a() + 360. : new_b);
    }
    else {
        auto new_b = PointLonLat::normalise_angle_to_maximum(b() + DB, a()) - DB;
        periodic_  = types::is_approximately_lesser_or_equal(360., (a() - new_b) * (1. + 1. / static_cast<double>(n)))
                         ? PeriodicNoEndPoint
                         : NonPeriodic;
        b(periodic() ? a() - 360. : new_b);
    }
}


Range* RegularLongitude::flip() const {
    auto flipped = periodic_ == PeriodicNoEndPoint     ? PeriodicNoStartPoint
                   : periodic_ == PeriodicNoStartPoint ? PeriodicNoEndPoint
                                                       : NonPeriodic;
    return new RegularLongitude(size(), b(), a(), flipped, eps());
}


Fraction RegularLongitude::increment() const {
    return Fraction((b() - a()) / static_cast<double>(periodic() ? size() : (size() - 1)));
}


Range* RegularLongitude::crop(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b) ||
           (types::is_approximately_equal(a(), b(), eps()) && types::is_approximately_equal(crop_a, crop_b, eps())));

    auto n = size();
    crop_b = PointLonLat::normalise_angle_to_minimum(crop_b - DB, crop_a) + DB;

    if (types::is_approximately_equal(crop_a, crop_b, eps())) {
        NOTIMP;  // FIXME
    }
    else if (a() < b()) {
        auto inc(increment());
        auto da = (a() / inc).decimalPart() * inc;
        auto af = regular_adjust(crop_a - da, inc, true) + da;
        auto bf = regular_adjust(crop_b - da, inc, false) + da;

        if (bf - af + inc >= 360) {
            return new RegularLongitude(n, af, af + 360);
        }

        auto nf = (bf - af) / inc;
        ASSERT(nf.integer() && nf <= n);

        return new RegularLongitude(static_cast<size_t>(nf.integralPart() + 1), af, bf);
    }
    else {
        NOTIMP;  // FIXME
    }
}


const std::vector<double>& RegularLongitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), !periodic());
        ASSERT(!values_.empty());
    }

    return values_;
}


}  // namespace range


}  // namespace eckit::geo
