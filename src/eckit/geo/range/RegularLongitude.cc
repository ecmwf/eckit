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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


RegularLongitude::RegularLongitude(double _inc, double _a, double _b, double _ref, double _eps) :
    Regular(_inc, _a, _b, _ref, _eps) {
    const Fraction period(360, 1);
    const Fraction inc(_inc);

    auto n = size();
    if ((n - 1) * inc >= period) {
        n -= 1;
    }

    ASSERT(n * inc <= period);
    resize(n);

    periodic((n + 1) * inc >= period);
    b(Fraction(a()) + n * inc);
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double _eps) :
    Regular(n, _a, _b, _eps) {
    static constexpr auto DB = 1e-12;

    if (a() < b()) {
        auto new_b = PointLonLat::normalise_angle_to_minimum(b() - DB, a()) + DB;
        if (types::is_approximately_lesser_or_equal(360., (new_b - a()) * (1. + 1. / static_cast<double>(n)))) {
            periodic(true);
            b(a() + 360.);
        }
        else {
            b(new_b);
        }
    }
    else {
        auto new_b = PointLonLat::normalise_angle_to_maximum(b() + DB, a()) - DB;
        if (types::is_approximately_lesser_or_equal(360., (a() - new_b) * (1. + 1. / static_cast<double>(n)))) {
            periodic(true);
            b(a() - 360.);
        }
        else {
            b(new_b);
        }
    }
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
        auto _a = adjust(crop_a - d, inc, true) + d;
        auto _b = adjust(crop_b - d, inc, false) + d;

        auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        auto n = static_cast<size_t>(nf.integralPart() + (nf * inc >= 360 ? 0 : 1));
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
