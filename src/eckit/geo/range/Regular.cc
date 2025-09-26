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


namespace {


Fraction adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
}


}  // namespace


Range* Regular::make_regular_xy_range(double inc, double a, double b, double ref) {
    ASSERT(a <= b);
    ASSERT(0 <= inc);

    const Fraction incf{inc};
    if (incf == 0 || types::is_approximately_equal(a, b)) {
        return new Regular(a, a, 1, true);
    }

    const Fraction reff{ref};
    const Fraction shift = (reff / incf).decimalPart() * incf;

    auto af = shift + adjust(Fraction(a) - shift, incf, true);
    auto bf = shift + adjust(Fraction(b) - shift, incf, false);
    ASSERT(af < bf);

    auto num = static_cast<size_t>((Fraction::abs(bf - af) / incf).integralPart() + 1);

    return new Regular(af, bf, num, true);
}


Range* Regular::make_regular_latitude_range(double inc, double a, double b, double ref) {
    ASSERT(SOUTH_POLE.lat <= a && a <= b && b <= NORTH_POLE.lat);

    return make_regular_xy_range(inc, a, b, ref);
}


Range* Regular::make_regular_longitude_range(double inc, double a, double b, double ref) {
    NOTIMP;
}


#if 0
const std::vector<double>& Regular::values() const {
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), !periodic_);
        ASSERT(!values_.empty());
    }

    return values_;
}






RegularLongitude::RegularLongitude(double _inc, double _a, double _b, double _ref) : Regular(_inc, _a, _b, _ref) {
    ASSERT(!types::is_approximately_equal(_a, _b));
    ASSERT(_a < _b);  // FIXME temporary

static const Fraction PERIOD(360, 1);

    auto n = 1 + (std::min(Fraction(b() - a()), PERIOD) / increment()).integralPart();
    auto p = n * increment() >= PERIOD;

    while (n * increment() >= PERIOD) {
        --n;
    }

    setPeriodic(p);
    resize(p ? n + 1 : n);
}
#endif


}  // namespace eckit::geo::range
