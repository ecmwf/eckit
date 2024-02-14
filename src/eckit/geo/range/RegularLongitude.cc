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


#include "eckit/geo/range/RegularLongitude.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


static util::recursive_mutex MUTEX;
static constexpr auto DB = 1e-12;


static Fraction regular_adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
};


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

    if (types::is_approximately_equal(crop_a, crop_b, eps())) {
        NOTIMP;  // FIXME
    }
    else if (a() < b()) {
        ASSERT(a() <= crop_a && crop_b <= b());  // FIXME do better

        auto inc(increment());
        auto d  = (a() / inc).decimalPart() * inc;
        auto _a = regular_adjust(crop_a - d, inc, true) + d;
        auto _b = regular_adjust(crop_b - d, inc, false) + d;

        auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        auto n = static_cast<size_t>(nf.integralPart() + (nf * inc >= 360 ? 0 : 1));
        ASSERT(0 < n && n <= size());

        return new RegularLongitude(n, _a, _b);
    }
    else {
        NOTIMP;  // FIXME
    }
    NOTIMP;
}


const std::vector<double>& RegularLongitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), !periodic());
        ASSERT(!values_.empty());
    }

    return values_;
}


}  // namespace eckit::geo::range
