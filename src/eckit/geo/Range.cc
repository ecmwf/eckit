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


namespace range {


namespace {


util::recursive_mutex MUTEX;

constexpr auto DB = 1e-12;


double pole_snap(double lat, double _eps) {
    return types::is_approximately_equal(lat, 90., _eps)    ? 90.
           : types::is_approximately_equal(lat, -90., _eps) ? -90.
                                                            : lat;
}


}  // namespace


GaussianLatitude::GaussianLatitude(size_t N, double _a, double _b, double _eps) :
    Range(2 * N, pole_snap(_a, _eps), pole_snap(_b, _eps), _eps), N_(N) {
    // pre-calculate on cropping
    if (auto [min, max] = std::minmax(a(), b()); -90. < min || max < 90.) {
        values_ = util::gaussian_latitudes(N_, a() < b());
        auto& v = values_;

        auto [from, to] = util::monotonic_crop(v, min, max, eps());
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);

        ASSERT(!v.empty());
        resize(v.size());

        a(v.front());
        b(v.back());
    }
}


const std::vector<double>& GaussianLatitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    return values_.empty() ? util::gaussian_latitudes(N_, a() < b()) : values_;
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double _eps) :
    Range(n, _a, types::is_approximately_equal(_a, _b, _eps) ? _a : _b, _eps) {
    // adjust range limits, before cropping
    if (types::is_approximately_equal(a(), b(), eps())) {
        resize(1);
        periodic_ = false;
        values_   = {a()};
    }
    else if (a() < b()) {
        auto x = PointLonLat::normalise_angle_to_minimum(b() - DB, a()) + DB;
        auto d = x - a();

        periodic_ = types::is_approximately_lesser_or_equal(360., d + d / static_cast<double>(n));
        b(periodic_ ? a() + 360. : x);
    }
    else {
        auto x = PointLonLat::normalise_angle_to_maximum(b() + DB, a()) - DB;
        auto d = a() - x;

        periodic_ = types::is_approximately_lesser_or_equal(360., d + d / static_cast<double>(n));
        b(periodic_ ? a() - 360. : x);
    }
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double crop_a, double crop_b, double _eps) :
    Range(n, _a, types::is_approximately_equal(_a, _b, _eps) ? _a : _b, _eps) {
    // adjust range limits, before cropping
    if (types::is_approximately_equal(a(), b(), eps())) {
        resize(1);
        periodic_ = false;
        values_   = {a()};
    }
    else if (a() < b()) {
        if (auto x = PointLonLat::normalise_angle_to_minimum(b() - DB, a()) + DB, d = x - a();
            (periodic_ = types::is_approximately_lesser_or_equal(360., d + d / static_cast<double>(n)))) {
            b(a() + 360.);

            crop_b = PointLonLat::normalise_angle_to_minimum(crop_b - DB, crop_a) + DB;

            auto adjust = [](const Fraction& target, const Fraction& inc, bool up) -> Fraction {
                ASSERT(inc > 0);

                auto r = target / inc;
                auto n = r.integralPart();

                if (!r.integer() && (target > 0) == up) {
                    n += (up ? 1 : -1);
                }

                return (n * inc);
            };

            Fraction inc((b() - a()) / static_cast<double>(periodic_ ? n : (n - 1)));
            auto da = (Fraction(a()) / inc).decimalPart() * inc;

            auto fraction_a = adjust(crop_a - da, inc, true) + da;
            auto fraction_b = adjust(crop_b - da, inc, false) + da;
            auto fraction_n = (fraction_b - fraction_a) / inc;
            ASSERT(fraction_n.integer() && 0 < fraction_n && fraction_n <= n);

            if (fraction_b - fraction_a < 360) {
                resize(static_cast<size_t>(fraction_n.integralPart()) + 1);
                periodic_ = false;
            }

            a(fraction_a);
            b(fraction_b);
        }
        else {
            b(x);

            NOTIMP;  // FIXME
        }
    }
    else {
        if (auto x = PointLonLat::normalise_angle_to_maximum(b() + DB, a()) - DB, d = a() - x;
            (periodic_ = types::is_approximately_lesser_or_equal(360., d + d / static_cast<double>(n)))) {
            b(a() - 360.);

            NOTIMP;  // FIXME
        }
        else {
            b(x);

            NOTIMP;  // FIXME
        }
    }
}


const std::vector<double>& RegularLongitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), !periodic_);
        ASSERT(!values_.empty());
    }

    return values_;
}


}  // namespace range


}  // namespace eckit::geo
