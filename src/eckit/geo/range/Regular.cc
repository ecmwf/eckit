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
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


namespace detail {


Fraction adjust(const Fraction& target, const Fraction& inc, bool up) {
    auto r = target / inc;
    auto n = r.integralPart() + ((r.integer() || (r > 0) != up) ? 0 : up ? 1 : -1);

    return n * inc;
};


size_t N(Fraction inc, Fraction a, Fraction b) {
    return static_cast<size_t>(inc == 0 ? 1 : (Fraction::abs((b - a) / inc)).integralPart() + 1);
}


struct RegularXY final : Regular::Implementation {
    using Implementation::Implementation;
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
        if (size_ == 1) {
            return;
        }

        if (Fraction::abs(b_ - a_ + inc_) >= PERIOD) {
            b_        = a_ + adjust(PERIOD, inc_, a_ < b_) - inc_;
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
        size_ = 1;
        inc_  = {};
        b_    = a_;
        return;
    }

    shift_ = (ref / inc).decimalPart() * inc;
    a_     = shift_ + detail::adjust(a_ - shift_, inc_, true);
    b_     = shift_ + detail::adjust(b_ - shift_, inc_, false);
    ASSERT(a < b == a_ < b_);

    size_ = detail::N(inc_, a_, b_);
    ASSERT(size_ > 0);
}


Regular* Regular::Implementation::make_cropped_range(double crop_a, double crop_b) const {
    const bool up = a_ <= b_;
    ASSERT(up ? crop_a <= crop_b : crop_a > crop_b);

    detail::RegularXY other(inc_, Fraction{crop_a}, Fraction{crop_b}, a_);
    auto _min = std::max(min(), other.min());
    auto _max = std::min(max(), other.max());

    return new range::RegularXY{inc_, up ? _min : _max, up ? _max : _min, a_};
}


bool Regular::Implementation::periodic() const {
    NOTIMP;
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
    static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

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


RegularXY::RegularXY(Fraction inc, Fraction _a, Fraction _b, Fraction ref) :
    Regular(new detail::RegularXY(inc, _a, _b, ref)) {
    ASSERT(types::is_approximately_equal(a(), b()) ? size() == 1 : size() > 1);
}


RegularXY* RegularXY::make_cropped_range(double crop_a, double crop_b) const {
    auto* ptr = dynamic_cast<RegularXY*>(Regular::make_cropped_range(crop_a, crop_b));
    ASSERT(ptr != nullptr);
    return ptr;
}


}  // namespace eckit::geo::range
