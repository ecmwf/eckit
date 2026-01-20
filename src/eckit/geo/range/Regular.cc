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
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


Regular::Regular(double _inc, double _a, double _b, double _ref, double eps) :
    Range(2, _a, _b, eps), increment_(_inc), periodic_(false) {
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


Regular::Regular(size_t n, double a, double b, bool periodic, double eps) :
    Range(n, a, b, eps),  //
    increment_(types::is_approximately_equal(a, b)
                   ? 0.
                   : (b - a) / static_cast<double>(std::max<size_t>(1, periodic ? n : n - 1))),
    periodic_(periodic) {}


Regular::Regular(size_t n, double a, double b, std::vector<double>&& values, bool periodic, double eps) :
    Range(n, a, b, eps),  //
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


}  // namespace eckit::geo::range
