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


#include "eckit/geo/range/Regular.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


Regular::Regular(double _inc, double _a, double _b, double _ref) :
    Range(2, _a, _b), increment_(_inc), periodic_(false) {
    ASSERT(0. <= _inc);

    if (increment_ == 0 || types::is_approximately_equal(_a, _b)) {
        b(_a);
        resize(1);
        return;
    }

    bool up    = _a < _b;
    auto shift = (Fraction(_ref) / increment_).decimalPart() * increment_;
    auto __a   = shift + adjust(Fraction(_a) - shift, increment_, up);
    auto __b   = shift + adjust(Fraction(_b) - shift, increment_, !up);
    auto n     = static_cast<size_t>((Fraction::abs(__b - __a) / increment_).integralPart() + 1);

    a(__a);
    b(__b);
    resize(n);
}


Regular::Regular(size_t n, double a, double b, bool periodic) :
    Range(n, a, b),  //
    increment_(types::is_approximately_equal(a, b)
                   ? 0.
                   : (b - a) / static_cast<double>(std::max<size_t>(1, periodic ? n : n - 1))),
    periodic_(periodic) {}


Regular::Regular(size_t n, double a, double b, std::vector<double>&& values, bool periodic) :
    Range(n, a, b),  //
    increment_(types::is_approximately_equal(a, b)
                   ? 0.
                   : (b - a) / static_cast<double>(std::max<size_t>(1, periodic ? n : n - 1))),
    values_(values),
    periodic_(periodic) {}


Fraction Regular::increment() const {
    return increment_;
}


const std::vector<double>& Regular::values() const {
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), !periodic_);
        ASSERT(!values_.empty());
    }

    return values_;
}


Fraction Regular::adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
}


static constexpr auto DB = 1e-12;


static Fraction regular_adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
};


RegularCartesian::RegularCartesian(size_t n, double a, double b) : Regular(n, a, b, false) {}


Range* RegularCartesian::make_range_cropped(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b) ||
           (types::is_approximately_equal(a(), b()) && types::is_approximately_equal(crop_a, crop_b)));

    if (types::is_approximately_equal(crop_a, crop_b)) {
        NOTIMP;  // FIXME
    }

    if (a() < b()) {
        ASSERT(a() <= crop_a && crop_b <= b());  // FIXME do better

        auto inc(increment());
        auto d  = (a() / inc).decimalPart() * inc;
        auto _a = regular_adjust(crop_a - d, inc, true) + d;
        auto _b = regular_adjust(crop_b - d, inc, false) + d;

        auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        auto n = static_cast<size_t>(nf.integralPart() + 1);
        ASSERT(0 < n && n <= size());

        return new RegularCartesian(n, _a, _b);
    }

    NOTIMP;
}


RegularLatitude::RegularLatitude(double _inc, double _a, double _b, double _ref) : Regular(_inc, _a, _b, _ref) {}


RegularLatitude::RegularLatitude(size_t n, double _a, double _b) : Regular(n, _a, _b, false) {
    ASSERT(SOUTH_POLE.lat <= a() && a() <= NORTH_POLE.lat);
    ASSERT(SOUTH_POLE.lat <= b() && b() <= NORTH_POLE.lat);
}


Range* RegularLatitude::make_range_cropped(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b) ||
           (types::is_approximately_equal(a(), b()) && types::is_approximately_equal(crop_a, crop_b)));

    if (types::is_approximately_equal(crop_a, crop_b)) {
        NOTIMP;  // FIXME
    }

    if (a() < b()) {
        ASSERT(a() <= crop_a && crop_b <= b());  // FIXME do better

        const auto inc(increment());
        const auto d  = (a() / inc).decimalPart() * inc;
        const auto _a = adjust(crop_a - d, inc, true) + d;
        const auto _b = adjust(crop_b - d, inc, false) + d;

        const auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        const auto n = static_cast<size_t>(nf.integralPart() + 1);
        ASSERT(0 < n && n <= size());

        return new RegularLatitude(n, _a, _b);
    }

    NOTIMP;
}


static const Fraction PERIOD(360, 1);


RegularLongitude::RegularLongitude(double _inc, double _a, double _b, double _ref) : Regular(_inc, _a, _b, _ref) {
    ASSERT(!types::is_approximately_equal(_a, _b));
    ASSERT(_a < _b);  // FIXME temporary

    auto n = 1 + (std::min(Fraction(b() - a()), PERIOD) / increment()).integralPart();
    auto p = n * increment() >= PERIOD;

    while (n * increment() >= PERIOD) {
        --n;
    }

    setPeriodic(p);
    resize(p ? n + 1 : n);
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b) :
    Regular(n, _a, _b, types::is_approximately_lesser_or_equal<double>(PERIOD, std::abs(_b - _a))) {}


Range* RegularLongitude::make_range_cropped(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b));

    if (a() < b()) {
        const auto inc(increment());

        if (periodic()) {
            return new RegularLongitude(inc, crop_a, crop_b, a());
        }

        RegularLongitude crop(inc, crop_a, crop_b, a());
        if (crop.periodic()) {
            auto _a = std::max(a(), PointLonLat::normalise_angle_to_minimum(crop_a, crop.a()));
            auto _b = PointLonLat::normalise_angle_to_minimum(crop_b, _a);
            return new RegularLongitude(inc, _a, _b, a());
        }

        ASSERT(a() <= crop_a && crop_b <= b());  // FIXME do better

        auto d  = (a() / inc).decimalPart() * inc;
        auto _a = adjust(crop_a - d, inc, true) + d;
        auto _b = adjust(crop_b - d, inc, false) + d;

        auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        auto n = static_cast<size_t>(nf.integralPart() + (nf * inc >= PERIOD ? 0 : 1));
        ASSERT(0 < n && n <= size());

        return new RegularLongitude(n, _a, _b);
    }

    NOTIMP;
}


}  // namespace eckit::geo::range
