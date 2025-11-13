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


Regular make_regular_range(double inc, double a, double b, double ref) {
    if (types::is_approximately_equal(inc, 0.) || types::is_approximately_lesser_or_equal((b - a) * inc, 0.)) {
        return Regular{Fraction{}, Fraction{a}, Fraction{a}};
    }

    const Fraction incf{inc};
    const Fraction reff{ref};
    const Fraction shift = (reff / incf).decimalPart() * incf;

    const auto af = shift + adjust(Fraction{a} - shift, incf, true);
    const auto bf = shift + adjust(Fraction{b} - shift, incf, false);
    ASSERT(a < b == af < bf);

    auto num = N(incf, af, bf);
    ASSERT(num > 0);

    return Regular{incf, af, num == 1 ? af : bf};
}


}  // namespace


Regular::Regular(double inc, double a, double b, double ref) : Regular{make_regular_range(inc, a, b, ref)} {}


Regular::Regular(Fraction inc, Fraction a, Fraction b) : Range(N(inc, a, b), a, b), increment_(inc), a_(a), b_(b) {
    ASSERT(size() >= 1);
}


Regular* Regular::make_xy_range(double inc, double a, double b, double ref) {
    return new Regular{make_regular_range(inc, a, b, ref)};
}


Regular* Regular::make_cropped_range(double crop_a, double crop_b) const {
    const bool up = a() <= b();
    ASSERT(up ? crop_a <= crop_b : crop_a > crop_b);

    Regular other(increment(), crop_a, crop_b, a());
    auto _min = std::max(min(), other.min());
    auto _max = std::min(max(), other.max());

    return new Regular{increment(), up ? _min : _max, up ? _max : _min, a()};
}


const std::vector<double>& Regular::values() const {
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    return util::linspace(a(), b(), size());
}


RegularLatitudeRange::RegularLatitudeRange(Regular&& regular) :
    LatitudeRange(regular.size(), regular.a(), regular.b()), regular_(regular) {
    ASSERT(SOUTH_POLE.lat <= min() && min() <= max() && max() <= NORTH_POLE.lat);
}


RegularLatitudeRange::RegularLatitudeRange(double inc, double a, double b, double ref) :
    RegularLatitudeRange(Regular(inc, a, b, ref)) {}


bool RegularLatitudeRange::includesNorthPole() const {
    return types::is_approximately_greater_or_equal<double>(max() + Fraction::abs(increment()), NORTH_POLE.lat);
}


bool RegularLatitudeRange::includesSouthPole() const {
    return types::is_approximately_lesser_or_equal<double>(min() - Fraction::abs(increment()), SOUTH_POLE.lat);
}


RegularLatitudeRange* RegularLatitudeRange::make_cropped_range(double crop_a, double crop_b) const {
    std::unique_ptr<Regular> cropped(regular_.make_cropped_range(crop_a, crop_b));
    return new RegularLatitudeRange(std::move(*cropped));
}


RegularLongitudeRange::RegularLongitudeRange(Regular&& regular) :
    LongitudeRange(regular.size(), regular.a(), regular.b()), regular_(regular), periodic_(false) {
    if (const static Fraction PERIOD(360, 1);
        Fraction::abs(regular_.bf() - regular_.af() + regular_.increment()) >= PERIOD) {
        auto af  = regular_.af();
        auto bf  = af + adjust(PERIOD, regular_.increment(), regular_.af() < regular_.bf()) - regular_.increment();
        auto n   = N(regular_.increment(), af, bf);
        regular_ = Regular{regular_.increment(), af, bf};

        resize(n);
        a(af);
        b(bf);
        periodic_ = true;
    }
}


RegularLongitudeRange::RegularLongitudeRange(double inc, double a, double b, double ref) :
    RegularLongitudeRange(Regular(inc, a, b, ref)) {}


RegularLongitudeRange* RegularLongitudeRange::make_cropped_range(double crop_a, double crop_b) const {
    if (periodic()) {
        return new RegularLongitudeRange(increment(), crop_a, crop_b, a());
    }

    std::unique_ptr<Regular> cropped(regular_.make_cropped_range(crop_a, crop_b));
    return new RegularLongitudeRange(std::move(*cropped));
}


}  // namespace eckit::geo::range
