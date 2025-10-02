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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


namespace {


Fraction adjust(const Fraction& target, const Fraction& inc, bool up) {
    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
};


size_t N(Fraction inc, Fraction a, Fraction b) {
    return static_cast<size_t>(inc == 0 ? 1 : (Fraction::abs((b - a) / inc)).integralPart() + 1);
}


}  // namespace


Regular::Regular(Fraction inc, Fraction a, Fraction b, bool periodic) :
    Range(N(inc, a, b), a, b), increment_(inc), a_(a), b_(b), periodic_(periodic) {
    ASSERT(size() >= 1);
}


Regular* Regular::make_xy_range(double inc, double a, double b, double ref) {
    if (types::is_approximately_equal(inc, 0.) || types::is_approximately_lesser_or_equal((b - a) * inc, 0.)) {
        return new Regular({}, Fraction{a}, Fraction{a}, false);
    }

    const Fraction incf{inc};
    const Fraction reff{ref};
    const Fraction shift = (reff / incf).decimalPart() * incf;

    const auto af = shift + adjust(Fraction{a} - shift, incf, true);
    const auto bf = shift + adjust(Fraction{b} - shift, incf, false);
    ASSERT(a < b == af < bf);

    auto num = N(incf, af, bf);
    ASSERT(num > 0);

    return new Regular(incf, af, num == 1 ? af : bf, false);
}


Regular* Regular::make_latitude_range(double inc, double a, double b, double ref) {
    ASSERT(SOUTH_POLE.lat <= a && a <= NORTH_POLE.lat);
    ASSERT(SOUTH_POLE.lat <= b && b <= NORTH_POLE.lat);

    return make_xy_range(inc, a, b, ref);
}


Regular* Regular::make_longitude_range(double inc, double a, double b, double ref) {
    std::unique_ptr<Regular> r(make_xy_range(inc, a, b, ref));
    ASSERT(r);

    if (const static Fraction PERIOD(360, 1); Fraction::abs(r->b_ - r->a_ + r->increment()) >= PERIOD) {
        auto period = adjust(PERIOD, r->increment(), a < b) - r->increment();
        return new Regular(r->increment(), r->a_, r->a_ + period, true);
    }

    return r.release();
}


Range* Regular::make_cropped_range(double crop_a, double crop_b) const {
    const bool up = a() <= b();
    ASSERT(up ? crop_a <= crop_b : crop_a > crop_b);

    if (periodic()) {
        return make_longitude_range(increment(), crop_a, crop_b, a());
    }

    std::unique_ptr<Regular> other(make_xy_range(increment(), crop_a, crop_b, a()));
    auto _min = std::max(min(), other->min());
    auto _max = std::min(max(), other->max());

    return make_xy_range(increment(), up ? _min : _max, up ? _max : _min, a());
}


const std::vector<double>& Regular::values() const {
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    return util::linspace(a(), b(), size());
}


}  // namespace eckit::geo::range
