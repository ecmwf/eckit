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
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


Regular::Regular(double _inc, double _a, double _b, double _ref, double eps) : Range(2, _a, _b, eps), periodic_(false) {
    ASSERT(0. <= _inc);

    Fraction inc(_inc);
    if (inc == 0 || types::is_approximately_equal(_a, _b)) {
        b(_a);
        resize(1);
        return;
    }

    bool up    = _a < _b;
    auto shift = (Fraction(_ref) / inc).decimalPart() * inc;
    auto __a   = shift + adjust(Fraction(_a) - shift, inc, up);
    auto __b   = shift + adjust(Fraction(_b) - shift, inc, !up);
    auto n     = static_cast<size_t>((Fraction::abs(__b - __a) / inc).integralPart() + 1);

    a(__a);
    b(__b);
    resize(n);
}


Fraction Regular::increment() const {
    ASSERT(1 < size());
    return Fraction(std::abs(b() - a()) / static_cast<double>(periodic() ? size() : (size() - 1)));
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
