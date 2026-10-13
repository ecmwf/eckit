// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/range/Regular.h"

#include <algorithm>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


namespace detail {


/// A multiple of inc, the nearest to target (up or down)
Fraction adjust(const Fraction& target, const Fraction& inc, bool up) {
    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
}


/// The offset of the values from the multiples of inc (as the reference), in [0, |inc|)
Fraction shift(const Fraction& ref, const Fraction& inc) {
    auto s = (ref / inc).decimalPart() * inc;
    return s < 0 ? s + Fraction::abs(inc) : s;
}


size_t N(Fraction inc, Fraction a, Fraction b) {
    return static_cast<size_t>(inc == 0 ? 1 : (Fraction::abs((b - a) / inc)).integralPart() + 1);
}


struct Plain final : Regular::Implementation {
    using Implementation::Implementation;
    explicit Plain(const Implementation& other) : Implementation(other) {}
};


struct Cropped final : Regular {
    explicit Cropped(Implementation* impl) : Regular(impl) {}
};


struct RegularLatitude final : Regular::Implementation {
    using Implementation::Implementation;

    Regular* make_cropped_range(double crop_a, double crop_b) const override {
        std::unique_ptr<Regular> x{Implementation::make_cropped_range(crop_a, crop_b)};
        return new range::RegularLatitude(x->increment(), x->af(), x->bf(), x->af());
    }

    bool includesNorthPole() const override {
        return types::is_approximately_greater_or_equal<double>(max() + Fraction::abs(inc_), NORTH_POLE.lat());
    }

    bool includesSouthPole() const override {
        return types::is_approximately_lesser_or_equal<double>(min() - Fraction::abs(inc_), SOUTH_POLE.lat());
    }
};


struct RegularLongitude final : Regular::Implementation {
    RegularLongitude(Fraction inc, Fraction a, Fraction b, Fraction ref) : Implementation(inc, a, b, ref) {
        // (a single value has no increment, so isn't periodic)
        if (Fraction::abs(b_ - a_ + inc_) >= PERIOD) {
            const auto inc  = Fraction::abs(inc_);
            const auto span = adjust(PERIOD, inc, true) - inc;

            if (a_ < b_) {
                b_ = a_ + span;
            }
            else {
                a_ = b_ + span;
            }

            size_     = N(inc_, a_, b_);
            periodic_ = true;
        }
    }

    Regular* make_cropped_range(double crop_a, double crop_b) const override {
        if (periodic()) {
            return new range::RegularLongitude(inc_, crop_a, crop_b, a_);
        }

        std::unique_ptr<Regular> x{Implementation::make_cropped_range(crop_a, crop_b)};

        return size_ == 0 ? range::RegularLongitude::make_empty_range(x->af(), x->bf())
                          : new range::RegularLongitude(x->increment(), x->af(), x->bf(), x->af());
    }

    static RegularLongitude* make_empty(Fraction a, Fraction b) {
        auto* impl(new RegularLongitude(b - a, a, b, a));
        ASSERT(impl != nullptr);

        impl->size_ = 0;
        return impl;
    }

    bool periodic() const override { return periodic_; }

    bool periodic_ = false;
    const static Fraction PERIOD;
};


const Fraction RegularLongitude::PERIOD{360, 1};


}  // namespace detail


Regular::Implementation::Implementation(Fraction inc, Fraction a, Fraction b, Fraction ref) : inc_(inc), a_(a), b_(b) {
    if (types::is_approximately_equal<double>(inc_, 0.) ||
        types::is_approximately_lesser_or_equal<double>((b_ - a_) * inc_, 0.)) {
        // no increment, or b not in its direction: a, as given
        b_ = a_;
    }
    else {
        // the values, multiples of inc from the reference, within [a, b]
        shift_ = detail::shift(ref, inc_);
        a_     = shift_ + detail::adjust(a_ - shift_, inc_, true);
        b_     = shift_ + detail::adjust(b_ - shift_, inc_, false);
        ASSERT(a_ == b_ || (a < b) == (a_ < b_));
    }

    size_ = detail::N(inc_, a_, b_);
    ASSERT(size_ > 0);

    // a single value has no increment (eg. a grid of a single point, row or column)
    if (size_ == 1) {
        inc_   = {};
        shift_ = {};
    }
}


Regular* Regular::Implementation::make_cropped_range(double crop_a, double crop_b) const {
    // a single value (without increment), as is
    if (size_ == 1) {
        return new detail::Cropped(new detail::Plain{*this});
    }

    const bool up = a_ <= b_;
    ASSERT(up ? crop_a <= crop_b : crop_a > crop_b);

    detail::Plain other(inc_, Fraction{crop_a}, Fraction{crop_b}, a_);
    const auto _min = std::max(min(), other.min());
    const auto _max = std::min(max(), other.max());

    return new detail::Cropped(new detail::Plain{inc_, up ? _min : _max, up ? _max : _min, a_});
}


bool Regular::Implementation::periodic() const {
    return false;
}


bool Regular::Implementation::includesNorthPole() const {
    NOTIMP;
}


bool Regular::Implementation::includesSouthPole() const {
    NOTIMP;
}


Fraction Regular::Implementation::min() const {
    return std::min(a_, b_);
}


Fraction Regular::Implementation::max() const {
    return std::max(a_, b_);
}


const std::vector<double>& Regular::values() const {
    lock_type lock;

    return util::linspace(a(), b(), size());
}


Regular::Regular(Implementation* impl) : impl_(impl) {
    ASSERT(impl_);
}


RegularLatitude::RegularLatitude(Fraction inc, Fraction _a, Fraction _b, Fraction ref) :
    Regular(new detail::RegularLatitude(inc, _a, _b, ref)) {
    ASSERT(types::is_approximately_equal(a(), b()) ? size() == 1 : size() > 1);
    ASSERT(SOUTH_POLE.lat() <= min() && min() <= max() && max() <= NORTH_POLE.lat());
}


RegularLatitude* RegularLatitude::make_cropped_range(double crop_a, double crop_b) const {
    auto* ptr = dynamic_cast<RegularLatitude*>(Regular::make_cropped_range(crop_a, crop_b));
    ASSERT(ptr != nullptr);
    return ptr;
}


RegularLongitude::RegularLongitude(Fraction inc, Fraction _a, Fraction _b, Fraction ref) :
    Regular(new detail::RegularLongitude(inc, _a, _b, ref)) {
    ASSERT(types::is_approximately_equal(a(), b()) ? size() == 1 : size() > 1);
}


RegularLongitude* RegularLongitude::make_empty_range(Fraction a, Fraction b) {
    return new RegularLongitude(detail::RegularLongitude::make_empty(a, b));
}


RegularLongitude* RegularLongitude::make_cropped_range(double crop_a, double crop_b) const {
    auto* ptr = dynamic_cast<RegularLongitude*>(Regular::make_cropped_range(crop_a, crop_b));
    ASSERT(ptr != nullptr);
    return ptr;
}


}  // namespace eckit::geo::range
