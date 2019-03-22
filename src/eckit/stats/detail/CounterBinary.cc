/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/detail/CounterBinary.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <ostream>
#include <sstream>

#include "mir/param/SameParametrisation.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace mir {
namespace stats {
namespace detail {


CounterBinary::CounterBinary(const param::MIRParametrisation& param1,
                             const param::MIRParametrisation& param2) :
    counter1_(param1),
    counter2_(param2),
    missing_(0),
    missingDifferent_(0),
    maxIndex_(0),
    countBelowLowerLimit_(0),
    countAboveUpperLimit_(0),
    max_(std::numeric_limits<double>::quiet_NaN()),
    lowerLimit_(std::numeric_limits<double>::quiet_NaN()),
    upperLimit_(std::numeric_limits<double>::quiet_NaN()),
    absoluteError_(std::numeric_limits<double>::quiet_NaN()),
    ignoreDifferentMissingValues_(0),
    ignoreAboveUpperLimit_(0),
    ignoreDifferentMissingValuesFactor_(std::numeric_limits<double>::quiet_NaN()),
    ignoreAboveUpperLimitFactor_(std::numeric_limits<double>::quiet_NaN()),
    toleranceType_(absTolerance_t::NONE),
    first_(true) {

    std::unique_ptr<param::MIRParametrisation> same(new param::SameParametrisation(param1, param2));

    same->get("ignore-different-missing-values", ignoreDifferentMissingValues_);
    same->get("ignore-different-missing-values-factor", ignoreDifferentMissingValuesFactor_);

    same->get("counter-lower-limit", lowerLimit_);
    same->get("counter-upper-limit", upperLimit_);
    same->get("ignore-above-upper-limit", ignoreAboveUpperLimit_);
    same->get("ignore-above-upper-limit-factor", ignoreAboveUpperLimitFactor_);

    hasLowerLimit_ = lowerLimit_ == lowerLimit_;
    hasUpperLimit_ = upperLimit_ == upperLimit_;

    absoluteError_ = std::numeric_limits<double>::quiet_NaN();
    if (same->get("absolute-error", absoluteError_)) {
        toleranceType_ = ABSOLUTE;
        return;
    }

    relativeErrorMin_ = std::numeric_limits<double>::quiet_NaN();
    relativeErrorMax_ = std::numeric_limits<double>::quiet_NaN();
    if (same->get("relative-error-min", relativeErrorMin_) ||
        same->get("relative-error-max", relativeErrorMax_)) {
        toleranceType_ = RELATIVE;
        return;
    }

    double pef = std::numeric_limits<double>::quiet_NaN();
    if (same->get("packing-error-factor", pef)) {
        ASSERT(pef > 0.);
        double packingError = 0.;

        double value = 0.;
        if (param1.fieldParametrisation().get("packingError", value) && value > 0.) {
            packingError = value;
        }
        if (param2.fieldParametrisation().get("packingError", value) && value > packingError) {
            packingError = value;
        }

        if (eckit::types::is_strictly_greater(packingError, 0.)) {
            absoluteError_ = pef * packingError;
            toleranceType_ = PACKINGERROR;
            return;
        }
    }
}


void CounterBinary::reset(const data::MIRField& field1, const data::MIRField& field2) {
    counter1_.reset(field1);
    counter2_.reset(field2);

    missing_ = 0;
    missingDifferent_ = 0;
    maxIndex_ = 0;
    countBelowLowerLimit_ = 0;
    countAboveUpperLimit_ = 0;

    max_ = std::numeric_limits<double>::quiet_NaN();
    first_ = true;
}


void CounterBinary::print(std::ostream& out) const {
    out << "CounterBinary["
            "count=" << count()
        << ",max=" << max()
        << ",maxIndex=" << maxIndex()
        << ",missing=" << missing()
        << ",missingDifferent=" << missingDifferent();
    if (hasUpperLimit_) {
        out << ",countAboveUpperLimit=" << countAboveUpperLimit();
    }
    if (hasLowerLimit_) {
        out << ",countBelowLowerLimit=" << countBelowLowerLimit();
    }
    if (toleranceType_ == ABSOLUTE || toleranceType_ == PACKINGERROR) {
        out << ",toleranceType=" << (toleranceType_ == ABSOLUTE ? "Absolute" : "packingError")
            << ",tolerance=" << absoluteError_;
    }
    else if (toleranceType_ == RELATIVE) {
        out << ",toleranceType=Relative";
        if (relativeErrorMin_ == relativeErrorMin_) {
            out << ",toleranceMin=" << relativeErrorMin_;
        }
        if (relativeErrorMax_ == relativeErrorMax_) {
            out << ",toleranceMax=" << relativeErrorMax_;
        }
    }
    out << ",counter1=" << counter1_
        << ",counter2=" << counter2_
        << "]";
}


bool CounterBinary::count(const double& a, const double& b, const double& diff) {
    size_t index = counter1_.count();
    bool miss1 = !counter1_.count(a);
    bool miss2 = !counter2_.count(b);

    if (!miss1 && !miss2) {

        if (hasUpperLimit_ && diff > upperLimit_) {
            ++countAboveUpperLimit_;
        }
        if (hasLowerLimit_ && diff < lowerLimit_) {
            ++countBelowLowerLimit_;
        }

        if (max_ < diff || first_) {
            max_ = diff;
            maxIndex_ = index;
        }

        first_ = false;
        return true;
    }

    (miss1 && miss2 ? missing_ : missingDifferent_)++;
    return false;
}


std::string CounterBinary::check() const {
    ASSERT(count());
    std::ostringstream reasons;

    if (missingDifferent_ > ignoreDifferentMissingValues()) {
        reasons << "\n" "* different missing values (" << missingDifferent() << ") greater than " << ignoreDifferentMissingValues();
    }

    if (countAboveUpperLimit_ > ignoreAboveUpperLimit()) {
        reasons << "\n" "* counter above limit " << upperLimit_ << " (" << countAboveUpperLimit() << ") greater than " << ignoreAboveUpperLimit();
    }

    if (toleranceType_ == ABSOLUTE || toleranceType_ == PACKINGERROR) {
        if (max_ > absoluteError_) {
            reasons << "\n" "* maximum difference (" << max_ << ") greater than " << absoluteError_;
        }
    }

    if (toleranceType_ == RELATIVE) {
        auto relative_error = [](double a, double b) -> double {
            double mx = std::max(std::abs(a), std::abs(b));
            if (eckit::types::is_approximately_equal(mx, 0.)) {
                mx = 1.;
            }
            return std::abs(a - b) / mx;
        };

        double relErrorMin = relative_error(counter1_.min(), counter2_.min());
        if (relErrorMin > relativeErrorMin_) {
            reasons << "\n" "* minimum relative error (" << relErrorMin << ") greater than " << relativeErrorMin_;
        }

        double relErrorMax = relative_error(counter1_.max(), counter2_.max());
        if (relErrorMax > relativeErrorMax_) {
            reasons << "\n" "* maximum relative error (" << relErrorMax << ") greater than " << relativeErrorMax_;
        }
    }

    return reasons.str();
}


size_t CounterBinary::ignoreAboveUpperLimit() const {
    return ignoreAboveUpperLimitFactor_ == ignoreAboveUpperLimitFactor_ ?
                size_t(double(count()) * ignoreAboveUpperLimitFactor_) :
                ignoreAboveUpperLimit_;
}


size_t CounterBinary::ignoreDifferentMissingValues() const {
    return ignoreDifferentMissingValuesFactor_ == ignoreDifferentMissingValuesFactor_ ?
                size_t(double(count()) * ignoreDifferentMissingValuesFactor_) :
                ignoreDifferentMissingValues_;
}


size_t CounterBinary::count() const {
    ASSERT(counter1_.count() == counter2_.count());
    return counter1_.count();
}


size_t CounterBinary::countAboveUpperLimit() const {
    return countAboveUpperLimit_;
}


size_t CounterBinary::countBelowLowerLimit() const {
    return countBelowLowerLimit_;
}


double CounterBinary::max() const {
    return max_;
}


size_t CounterBinary::maxIndex() const {
    return maxIndex_;
}


size_t CounterBinary::missing() const {
    return missing_;
}


size_t CounterBinary::missingDifferent() const {
    return missingDifferent_;
}


}  // namespace detail
}  // namespace stats
}  // namespace mir
