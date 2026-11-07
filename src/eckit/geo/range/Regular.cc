// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/range/Regular.h"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


namespace {


struct lock_tag;  // of this file's mutex
using lock_type = util::lock_type<lock_tag>;


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


/// The first and last values (multiples of inc from shift) within [lo, hi], if any; values at the bounds (within
/// PointLonLat::EPS, as Gaussian latitudes crops) are within
std::optional<std::pair<Fraction, Fraction>> within(Fraction inc, const Fraction& shift, double lo, double hi) {
    inc = Fraction::abs(inc);

    auto first = shift + adjust(Fraction{lo} - shift, inc, true);
    auto last  = shift + adjust(Fraction{hi} - shift, inc, false);

    if (types::is_approximately_equal<double>(first - inc, lo, PointLonLat::EPS)) {
        first = first - inc;
    }

    if (types::is_approximately_equal<double>(last + inc, hi, PointLonLat::EPS)) {
        last = last + inc;
    }

    return first <= last ? std::make_optional(std::make_pair(first, last)) : std::nullopt;
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
        return x->size() == 0 ? range::RegularLatitude::make_empty_range(x->af(), x->bf())
                              : new range::RegularLatitude(x->increment(), x->af(), x->bf(), x->af());
    }

    static RegularLatitude* make_empty(Fraction a, Fraction b) {
        auto* impl(new RegularLatitude(b - a, a, b, a));
        ASSERT(impl != nullptr);

        impl->size_ = 0;
        return impl;
    }

    // at the pole, or past it by the next value (not at it, which would be a value of its own)
    bool includesNorthPole() const override {
        const double max = this->max();
        return types::is_approximately_equal(max, NORTH_POLE.lat()) ||
               types::is_strictly_greater<double>(max + Fraction::abs(inc_), NORTH_POLE.lat());
    }

    bool includesSouthPole() const override {
        const double min = this->min();
        return types::is_approximately_equal(min, SOUTH_POLE.lat()) ||
               types::is_strictly_greater<double>(SOUTH_POLE.lat(), min - Fraction::abs(inc_));
    }
};


struct RegularLongitude final : Regular::Implementation {
    RegularLongitude(Fraction inc, Fraction a, Fraction b, Fraction ref) : Implementation(inc, a, b, ref) {
        // (a single value has no increment, so isn't periodic)
        if (size_ > 1 && Fraction::abs(b_ - a_ + inc_) >= PERIOD) {
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
            if (!types::is_approximately_equal(crop_a, crop_b)) {
                return new range::RegularLongitude(inc_, crop_a, crop_b, a_);
            }

            // a single value (of any period), if one of the values
            const auto value = PointLonLat::normalise_angle_to_minimum(crop_a, min());
            if (auto w = within(inc_, shift_, value, value)) {
                return new range::RegularLongitude(Fraction{0}, w->first, w->first, w->first);
            }

            return range::RegularLongitude::make_empty_range(crop_a, crop_b);
        }

        std::unique_ptr<Regular> x{Implementation::make_cropped_range(crop_a, crop_b)};

        return x->size() == 0 ? range::RegularLongitude::make_empty_range(x->af(), x->bf())
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
        // no increment, or b not in its direction (e.g. a = b): a single value, as given (e.g. a grid of a single
        // point, row or column)
        b_    = a_;
        size_ = 1;
    }
    else {
        // the values, multiples of inc from the reference, within [a, b] (none if between values)
        shift_ = detail::shift(ref, inc_);

        const auto first = shift_ + detail::adjust(a_ - shift_, inc_, true);
        const auto last  = shift_ + detail::adjust(b_ - shift_, inc_, false);

        if ((last - first) * inc_ >= 0) {
            a_    = first;
            b_    = last;
            size_ = detail::N(inc_, a_, b_);
        }
    }

    // a single value (or none) has no increment
    if (size_ <= 1) {
        inc_   = {};
        shift_ = {};
    }
}


Regular* Regular::Implementation::make_cropped_range(double crop_a, double crop_b) const {
    // a single value (without increment), as is if within the crop
    if (size_ <= 1) {
        const double value = a_;

        auto* single = new detail::Plain{*this};
        if (size_ == 1 && !(types::is_approximately_lesser_or_equal(std::min(crop_a, crop_b), value) &&
                            types::is_approximately_lesser_or_equal(value, std::max(crop_a, crop_b)))) {
            single->size_ = 0;
        }
        return new detail::Cropped(single);
    }

    const bool up = a_ <= b_;
    ASSERT(up ? crop_a <= crop_b : crop_a >= crop_b);

    // the values within the crop (of these values, possibly none), in the same direction
    const auto lo = std::max<double>(min(), std::min(crop_a, crop_b));
    const auto hi = std::min<double>(max(), std::max(crop_a, crop_b));

    if (auto w = detail::within(inc_, shift_, lo, hi)) {
        const auto [first, last] = *w;
        return new detail::Cropped(new detail::Plain{inc_, up ? first : last, up ? last : first, a_});
    }

    auto* none  = new detail::Plain{*this};
    none->a_    = up ? Fraction{lo} : Fraction{hi};
    none->b_    = up ? Fraction{hi} : Fraction{lo};
    none->size_ = 0;
    return new detail::Cropped(none);
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
    if (size() > 0 && !(SOUTH_POLE.lat() <= min() && min() <= max() && max() <= NORTH_POLE.lat())) {
        throw exception::RangeError("RegularLatitude: latitudes in [-90, 90] expected", Here());
    }
}


RegularLatitude* RegularLatitude::make_empty_range(Fraction a, Fraction b) {
    return new RegularLatitude(detail::RegularLatitude::make_empty(a, b));
}


RegularLatitude* RegularLatitude::make_cropped_range(double crop_a, double crop_b) const {
    auto* ptr = dynamic_cast<RegularLatitude*>(Regular::make_cropped_range(crop_a, crop_b));
    ASSERT(ptr != nullptr);
    return ptr;
}


RegularLongitude::RegularLongitude(Fraction inc, Fraction _a, Fraction _b, Fraction ref) :
    Regular(new detail::RegularLongitude(inc, _a, _b, ref)) {}


RegularLongitude* RegularLongitude::make_empty_range(Fraction a, Fraction b) {
    return new RegularLongitude(detail::RegularLongitude::make_empty(a, b));
}


RegularLongitude* RegularLongitude::make_cropped_range(double crop_a, double crop_b) const {
    auto* ptr = dynamic_cast<RegularLongitude*>(Regular::make_cropped_range(crop_a, crop_b));
    ASSERT(ptr != nullptr);
    return ptr;
}


}  // namespace eckit::geo::range
