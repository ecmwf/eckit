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

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"


namespace eckit::geo::range {


static util::recursive_mutex MUTEX;


Fraction Regular::increment() const {
    ASSERT(1 < size());
    return Fraction(std::abs(b() - a()) / static_cast<double>(periodic() ? size() : (size() - 1)));
}

const std::vector<double>& Regular::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), !periodic_);
        ASSERT(!values_.empty());
    }

    return values_;
}


Regular::Regular(size_t n, double a, double b, std::vector<double>&& values, bool periodic, double eps) :
    Range(n, a, b, eps), values_(values), periodic_(periodic) {}


DiscreteRange::DiscreteRange(double _a, double _b, double _inc, double _ref) :
    a(_a), b(_b), inc(_inc), periodic(false) {
    ASSERT(_a <= _b && a <= b);
    ASSERT(0 <= _inc && 0 <= inc);

    auto adjust = [](const Fraction& target, const Fraction& inc, bool up) -> Fraction {
        ASSERT(inc > 0);

        auto r = target / inc;
        auto n = r.integralPart();

        if (!r.integer() && (r > 0) == up) {
            n += (up ? 1 : -1);
        }

        return n * inc;
    };

    if (inc == 0) {
        b = a;
        n = 1;
        return;
    }

    auto shift = (Fraction(_ref) / inc).decimalPart() * inc;
    a          = shift + adjust(a - shift, inc, true);

    if (b == a) {
        b = a;
    }
    else {
        auto c = shift + adjust(b - shift, inc, false);
        c      = a + ((c - a) / inc).integralPart() * inc;
        b      = c < a ? a : c;
    }

    n = static_cast<size_t>(((b - a) / inc).integralPart() + 1);

    ASSERT(a <= b);
    ASSERT(1 <= n);
}


DiscreteRange::DiscreteRange(double _a, double _b, double _inc, double _ref, double _period) :
    DiscreteRange(_a, _b, _inc, _ref) {
    Fraction period(_period);
    ASSERT(period > 0);

    if ((n - 1) * inc >= period) {
        n -= 1;
        ASSERT(n * inc == period || (n - 1) * inc < period);

        b = a + (n - 1) * inc;
    }

    if (n * inc == period) {
        periodic = true;
        b        = a + n * inc;
    }
}


Fraction Regular::adjust(const Fraction& target, const Fraction& inc, bool up) {
    ASSERT(inc > 0);

    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
}


}  // namespace eckit::geo::range
