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
#include "eckit/geo/PointLonLat.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


static const Fraction PERIOD(360, 1);


RegularLongitude::RegularLongitude(double _inc, double _a, double _b, double _ref, double _eps) :
    Regular(_inc, _a, _b, _ref, _eps) {
    ASSERT(!types::is_approximately_equal(_a, _b));
    ASSERT(_a < _b);  // FIXME temporary
    const Fraction inc(_inc);

    auto n = 1 + (std::min(Fraction(b() - a()), PERIOD) / inc).integralPart();
    setPeriodic(n * inc >= PERIOD);

    if (periodic()) {
        b(a() + PERIOD);
        resize((PERIOD / inc).integralPart());
    }
    else {
        b(Fraction(a()) + (n - 1) * inc);
        resize(n);
    }
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double _eps) :
    Regular(n, _a, _b, types::is_approximately_lesser_or_equal<double>(PERIOD, std::abs(_b - _a)), _eps) {}


Range* RegularLongitude::make_range_cropped(double crop_a, double crop_b) const {
    ASSERT((a() < b() && crop_a <= crop_b) || (a() > b() && crop_a >= crop_b));

    if (a() < b()) {
        const auto inc(increment());

        if (periodic()) {
            return new RegularLongitude(inc, crop_a, crop_b, a(), eps());
        }

        RegularLongitude crop(inc, crop_a, crop_b, a(), eps());
        if (crop.periodic()) {
            auto _a = std::max(a(), PointLonLat::normalise_angle_to_minimum(crop_a, crop.a()));
            auto _b = PointLonLat::normalise_angle_to_minimum(crop_b, _a);
            return new RegularLongitude(inc, _a, _b, a(), eps());
        }

        ASSERT(a() <= crop_a && crop_b <= b());  // FIXME do better

        auto d  = (a() / inc).decimalPart() * inc;
        auto _a = adjust(crop_a - d, inc, true) + d;
        auto _b = adjust(crop_b - d, inc, false) + d;

        auto nf = (_b - _a) / inc;
        ASSERT(nf.integer());

        auto n = static_cast<size_t>(nf.integralPart() + (nf * inc >= PERIOD ? 0 : 1));
        ASSERT(0 < n && n <= size());

        return new RegularLongitude(n, _a, _b, eps());
    }

    NOTIMP;
}


Range* RegularLongitude::make_range_flipped() const {
    std::vector<double> flipped(size());
    const auto& v = values();
    std::reverse_copy(v.begin(), v.end(), flipped.begin());

    return new RegularLongitude(size(), b(), a(), eps());
}


}  // namespace eckit::geo::range
