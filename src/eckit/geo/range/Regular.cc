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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


namespace {


const Fraction LONGITUDE_PERIOD(360, 1);


Fraction adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
};


bool is_longitude_range_periodic(const Fraction& increment, const Fraction& a, const Fraction& b) {
    return b - a + increment >= LONGITUDE_PERIOD;
}


}  // namespace


Regular::Regular(double a, double b, size_t num, bool periodic) :
    Range(num, a, b),
    increment_(num > 1 ? (b - a) / static_cast<double>(num - (periodic ? 0 : 1)) : 0),
    periodic_(periodic) {}


Range* Regular::make_xy_range(double inc, double a, double b, double ref) {
    const bool up = a < b;
    if (types::is_approximately_equal(inc, 0.) || (up ? inc < 0 || (b - a) < inc : inc > 0 || (b - a) > inc)) {
        return new Regular(a, a, 1, true);
    }

    const Fraction incf{inc};
    const Fraction reff{ref};
    const Fraction shift = (reff / incf).decimalPart() * incf;

    const auto af = shift + adjust(Fraction{a} - shift, incf, up);
    const auto bf = shift + adjust(Fraction{b} - shift, incf, !up);
    ASSERT(up == af < bf);

    auto num = static_cast<size_t>((Fraction::abs(bf - af) / incf).integralPart() + 1);
    ASSERT(num > 0);

    return new Regular(af, num == 1 ? af : bf, num, false);
}


Range* Regular::make_latitude_range(double inc, double a, double b, double ref) {
    ASSERT(SOUTH_POLE.lat <= a && a <= NORTH_POLE.lat);
    ASSERT(SOUTH_POLE.lat <= b && b <= NORTH_POLE.lat);

    return make_xy_range(inc, a, b, ref);
}


Range* Regular::make_longitude_range(double inc, double a, double b, double ref) {
    std::unique_ptr<Regular> x(dynamic_cast<Regular*>(make_longitude_range(inc, a, b, ref)));
    ASSERT(x);

    // if (is_longitude_range_periodic(incf, af, Fraction{b})) {
    //     auto num = static_cast<size_t>((LONGITUDE_PERIOD / incf).integralPart());
    //     return new Regular(af, af + LONGITUDE_PERIOD, num, true);
    // }

    // const auto b      = a + std::min<double>(b, a + LONGITUDE_PERIOD);
    // const Fraction bf = shift + adjust(Fraction{b} - shift, incf, false);


#if 0
    auto n = 1 + (std::min(Fraction(b() - a()), LONGITUDE_PERIOD) / increment()).integralPart();
    auto p = n * increment() >= LONGITUDE_PERIOD;

    while (n * increment() >= LONGITUDE_PERIOD) {
        --n;
    }

    resize(p ? n + 1 : n);
#endif
}


Range* Regular::make_cropped_range(double crop_a, double crop_b) const {
    ASSERT(a() <= b() ? crop_a <= crop_b : crop_a > crop_b);
    ASSERT((0 <= increment()) == (crop_a <= crop_b));

    NOTIMP;

    if (periodic()) {
        return make_longitude_range(increment(), crop_a, crop_b, a());
    }

    // bool crop_periodic =

#if 0
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
#endif
}


const std::vector<double>& Regular::values() const {
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    return util::linspace(a(), b(), size(), !periodic_);
}


}  // namespace eckit::geo::range
