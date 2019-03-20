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
    tolerance_(std::numeric_limits<double>::quiet_NaN()),
    ignoreDifferentMissingValues_(0),
    ignoreCountAboveUpperLimit_(0),
    ignoreDifferentMissingValuesFactor_(std::numeric_limits<double>::quiet_NaN()),
    ignoreCountAboveUpperLimitFactor_(std::numeric_limits<double>::quiet_NaN()),
    toleranceType_(tolerance_t::NONE),
    first_(true) {

    std::unique_ptr<param::MIRParametrisation> same(new param::SameParametrisation(param1, param2));

    same->get("ignore-different-missing-values", ignoreDifferentMissingValues_);
    same->get("ignore-different-missing-values-factor", ignoreDifferentMissingValuesFactor_);

    same->get("counter-upper-limit", upperLimit_);
    same->get("ignore-count-above-upper-limit", ignoreCountAboveUpperLimit_);
    same->get("ignore-count-above-upper-limit-factor", ignoreCountAboveUpperLimitFactor_);

    same->get("counter-lower-limit", lowerLimit_);

    hasLowerLimit_ = lowerLimit_ == lowerLimit_;
    hasUpperLimit_ = upperLimit_ == upperLimit_;

    tolerance_ = std::numeric_limits<double>::quiet_NaN();
    double ref = std::numeric_limits<double>::quiet_NaN();
    double pef = std::numeric_limits<double>::quiet_NaN();

    if (same->get("absolute-tolerance", tolerance_)) {
        toleranceType_ = ABSOLUTE;
        return;
    }

    if (same->get("relative-tolerance", tolerance_)) {
        if (same->get("relative-reference", ref)) {
            toleranceType_ = RELATIVETOREFERENCE;
            tolerance_ *= ref;
        } else {
            toleranceType_ = RELATIVETOMAXIMUM;
        }
        return;
    }

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
            tolerance_ = pef * packingError;
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
    lowerLimit_ = std::numeric_limits<double>::quiet_NaN();
    upperLimit_ = std::numeric_limits<double>::quiet_NaN();
    first_ = true;
}


void CounterBinary::print(std::ostream& out) const {
    out << "CounterBinary["
            "count=" << count()
        << ",check=" << check()
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
    if (toleranceType_ == RELATIVETOMAXIMUM ) {
        out << ",toleranceType=RelativeToMaximum"
            << ",tolerance=" << (tolerance_ * max_);
    } else if (toleranceType_) {
        out << ",toleranceType=" << (toleranceType_==ABSOLUTE ? "Absolute" : "RelativeToReference")
            << ",tolerance=" << tolerance_;
    }
    out << ",counter1=" << counter1_
        << ",counter2=" << counter2_
        << "]";
}


bool CounterBinary::count(const double& v1, const double& v2) {

    bool miss1 = !counter1_.count(v1);
    bool miss2 = !counter2_.count(v2);

    if (!miss1 && !miss2) {
        double v = std::abs(v2 - v1);

        if (hasUpperLimit_ && v > upperLimit_) {
            ++countAboveUpperLimit_;
        }
        if (hasLowerLimit_ && v < lowerLimit_) {
            ++countBelowLowerLimit_;
        }

        return true;
    }

    (miss1 && miss2 ? missing_ : missingDifferent_)++;
    return false;
}


bool CounterBinary::check() const {
    ASSERT(count());
    auto& log = eckit::Log::info();
    std::ostringstream str;

    if (missingDifferent() > ignoreDifferentMissingValues()) {
        str << "\n" "* different missing values (" << missingDifferent() << ") greater than " << ignoreDifferentMissingValues_;
    }

    if (max_ > tolerance()) {
        str << "\n" "* maximum difference (" << max_ << ") greater than " << tolerance();
    }

    if (countAboveUpperLimit_ > ignoreCountAboveUpperLimit()) {
        str << "\n" "* counter above limit " << upperLimit_ << " (" << countAboveUpperLimit() << ") greater than " << ignoreCountAboveUpperLimit();
    }

    auto reasons(str.str());
    if (reasons.empty()) {
        return true;
    }

    log << "Comparison failed because:" << reasons << std::endl;
    return false;
}


double CounterBinary::tolerance() const {
    return toleranceType_==tolerance_t::RELATIVETOMAXIMUM ? tolerance_ * max_ : tolerance_;
}


size_t CounterBinary::ignoreCountAboveUpperLimit() const {
    if (ignoreCountAboveUpperLimitFactor_ == ignoreCountAboveUpperLimitFactor_) {
        return size_t(double(count()) * ignoreCountAboveUpperLimitFactor_);
    }
    return ignoreCountAboveUpperLimit_;
}


size_t CounterBinary::ignoreDifferentMissingValues() const {
    if (ignoreDifferentMissingValuesFactor_ == ignoreDifferentMissingValuesFactor_) {
        return size_t(double(count()) * ignoreDifferentMissingValuesFactor_);
    }
    return ignoreDifferentMissingValues_;
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
