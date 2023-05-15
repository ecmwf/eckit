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


#include "eckit/geo/concept/Range.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::concept {


    static Fraction adjust(const Fraction& target, const Fraction& inc, bool up)
    {
        ASSERT(inc > 0);

        auto r = target / inc;
        auto n = r.integralPart();

        if (!r.integer() && (r > 0) == up) {
            n += (up ? 1 : -1);
        }

        return (n * inc);
    }


    Range::Range(double _a, double _b, double _inc, double _ref, double period) :
        Range(_a, _b, _inc, _ref) {
        ASSERT(0 < period);

        if ((n_ - 1) * inc_ >= period) {
            n_ -= 1;
            ASSERT(n_ * inc_ == period || (n_ - 1) * inc_ < period);

            b_ = a_ + (n_ - 1) * inc_;
        }
    }


    Range::Range(double _a, double _b, double _inc, double _ref) {
        ASSERT(0 <= _inc);

        if (types::is_approximately_equal(_inc, 0.) || types::is_approximately_equal(_a, _b)) {
            a_ = b_ = _ref;
            n_      = 1;
            inc_ = Fraction{0};
            return;
        }

        ASSERT(_a <= _b); // FIXME remove

        inc_ = Fraction{_inc};

        const Fraction a(_a);
        const Fraction b(_b);
        const Fraction ref(_ref);

        auto shift = (ref / inc_).decimalPart() * inc_;
        a_ = shift + adjust(a - shift, inc_, true);

        auto c = shift + adjust(b - shift, inc_, false);
        c = a_ + ((c - a_) / inc_).integralPart() * inc_;
        b_     = c < a_ ? a_ : c;

        n_ = static_cast<size_t>(((b_ - a_) / inc_).integralPart() + 1);

        ASSERT(a_ <= b_);
        ASSERT(1 <= n_);
    }


}  // namespace eckit::geo::concept
