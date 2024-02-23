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

#include <algorithm>
#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


static const Fraction PERIOD(360, 1);


RegularLongitude::RegularLongitude(double _inc, double _a, double _b, double _ref, double _eps) :
    Regular(_inc, _a, _b, _ref, _eps) {
    const Fraction inc(_inc);

    auto n = size();
    if ((n - 1) * inc >= PERIOD) {
        n -= 1;
    }

    ASSERT(n * inc <= PERIOD);
    resize(n);

    periodic((n + 1) * inc >= PERIOD);
    b(Fraction(a()) + (periodic() ? n : n - 1) * inc);
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double _eps) :
    Regular(n, _a, _b, types::is_approximately_lesser_or_equal<double>(PERIOD, std::abs(_b - _a)), _eps) {}


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
        auto _a = adjust(crop_a - d, inc, true) + d;
        auto _b = adjust(crop_b - d, inc, false) + d;

        auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        auto n = static_cast<size_t>(nf.integralPart() + (nf * inc >= PERIOD ? 0 : 1));
        ASSERT(0 < n && n <= size());

        return new RegularLongitude(n, _a, _b, eps());
    }
    else {
        NOTIMP;  // FIXME
    }

    NOTIMP;
}


Range* RegularLongitude::flip() const {
    std::vector<double> flipped(size());
    const auto& v = values();
    std::reverse_copy(v.begin(), v.end(), flipped.begin());

    return new RegularLongitude(size(), b(), a(), eps());
}


}  // namespace eckit::geo::range
