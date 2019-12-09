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


CounterBinary::CounterBinary(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) :
    counter1_(param1),
    counter2_(param2),
    missing_(0),
    missingIn1NotIn2_(0),
    missingIn2NotIn1_(0),
    maxIndex_(0),
    countBelowLowerLimit_(0),
    countAboveUpperLimit_(0),
    max_(std::numeric_limits<double>::quiet_NaN()),
    maxIndexValue1_(std::numeric_limits<double>::quiet_NaN()),
    maxIndexValue2_(std::numeric_limits<double>::quiet_NaN()),
    packingError_(std::numeric_limits<double>::quiet_NaN()),
    first_(true) {

    std::unique_ptr<param::MIRParametrisation> same(new param::SameParametrisation(param1, param2, false));
    static const double nan = std::numeric_limits<double>::quiet_NaN();

    same->get("ignore-different-missing-values", ignoreDifferentMissingValues_ = 0);
    same->get("ignore-different-missing-values-factor", ignoreDifferentMissingValuesFactor_ = nan);
    same->get("counter-lower-limit", lowerLimit_ = nan);
    same->get("counter-upper-limit", upperLimit_ = nan);
    same->get("ignore-above-upper-limit", ignoreAboveUpperLimit_ = 0);
    same->get("ignore-above-upper-limit-factor", ignoreAboveUpperLimitFactor_ = nan);

    hasLowerLimit_ = lowerLimit_ == lowerLimit_;
    hasUpperLimit_ = upperLimit_ == upperLimit_;

    doAbsoluteCompare_    = same->get("absolute-error", absoluteError_ = nan);
    doRelativeCompare_    = same->get("relative-error-factor", relativeErrorFactor_ = nan);
    doRelativeMinCompare_ = same->get("relative-error-min", relativeErrorMin_ = nan);
    doRelativeMaxCompare_ = same->get("relative-error-max", relativeErrorMax_ = nan);

    double pef = nan;
    if ((doPackingCompare_ = same->get("packing-error-factor", pef))) {
        double packingError1 = 0.;
        double packingError2 = 0.;
        ASSERT(param1.fieldParametrisation().get("packingError", packingError1) ||
               param2.fieldParametrisation().get("packingError", packingError2));

        packingError_ = pef * std::max(packingError1, packingError2);
        ASSERT(packingError_ > 0.);
    }
}


void CounterBinary::reset(const data::MIRField& field1, const data::MIRField& field2) {
    counter1_.reset(field1);
    counter2_.reset(field2);

    missing_              = 0;
    missingIn1NotIn2_     = 0;
    missingIn2NotIn1_     = 0;
    maxIndex_             = 0;
    countBelowLowerLimit_ = 0;
    countAboveUpperLimit_ = 0;

    max_            = std::numeric_limits<double>::quiet_NaN();
    maxIndexValue1_ = std::numeric_limits<double>::quiet_NaN();
    maxIndexValue2_ = std::numeric_limits<double>::quiet_NaN();
    first_          = true;
}


void CounterBinary::print(std::ostream& out) const {
    out << "CounterBinary["
           "count=" << count()
        << ",max=" << max()
        << ",maxIndex=" << maxIndex()
        << ",maxIndexValue1=" << maxIndexValue1_
        << ",maxIndexValue2=" << maxIndexValue2_
        << ",missing=" << missing()
        << ",missingIn1NotIn2=" << missingIn1NotIn2()
        << ",missingIn2NotIn1=" << missingIn2NotIn1();

    if (hasUpperLimit_) {
        out << ",countAboveUpperLimit=" << countAboveUpperLimit();
    }

    if (hasLowerLimit_) {
        out << ",countBelowLowerLimit=" << countBelowLowerLimit();
    }

    if (doAbsoluteCompare_) {
        out << ",toleranceType=Absolute,tolerance=" << absoluteError_;
    }

    if (doPackingCompare_) {
        out << ",toleranceType=PackingError,tolerance=" << packingError_;
    }

    if (doRelativeCompare_) {
        out << ",toleranceType=RelativeError,toleranceFactor=" << relativeErrorFactor_;
    }

    if (doRelativeMinCompare_) {
        out << ",toleranceType=RelativeMin,tolerance=" << relativeErrorMin_;
    }

    if (doRelativeMaxCompare_) {
        out << ",toleranceType=RelativeMax,tolerance=" << relativeErrorMax_;
    }

    out << ",counter1=" << counter1_ << ",counter2=" << counter2_ << "]";
}


bool CounterBinary::count(double a, double b, double diff) {
    size_t index = counter1_.count();
    bool miss1   = !counter1_.count(a);
    bool miss2   = !counter2_.count(b);

    if (!miss1 && !miss2) {

        if (hasUpperLimit_ && diff > upperLimit_) {
            ++countAboveUpperLimit_;
        }
        if (hasLowerLimit_ && diff < lowerLimit_) {
            ++countBelowLowerLimit_;
        }

        if (max_ < diff || first_) {
            max_            = diff;
            maxIndex_       = index;
            maxIndexValue1_ = a;
            maxIndexValue2_ = b;
        }

        first_ = false;
        return true;
    }

    (miss1 && miss2 ? missing_ : (miss1 ? missingIn1NotIn2_ : missingIn2NotIn1_))++;
    return false;
}


std::string CounterBinary::check() const {
    ASSERT(count());
    std::ostringstream reasons;

    auto missingDifferent = missingIn1NotIn2_ + missingIn2NotIn1_;
    if (missingDifferent > ignoreDifferentMissingValues()) {
        reasons << "\n" "* different missing values (" << missingDifferent << ") greater than " << ignoreDifferentMissingValues();
    }

    if (countAboveUpperLimit_ > ignoreAboveUpperLimit()) {
        reasons << "\n" "* counter above limit " << upperLimit_ << " (" << countAboveUpperLimit() << ") greater than " << ignoreAboveUpperLimit();
    }

    if (doAbsoluteCompare_) {
        if (max_ > absoluteError_) {
            reasons << "\n" "* maximum difference (" << max_ << ") greater than " << absoluteError_;
        }
    }

    if (doPackingCompare_) {
        if (max_ > packingError_) {
            reasons << "\n" "* maximum difference (" << max_ << ") greater than " << packingError_;
        }
    }

    auto relative_error = [](double a, double b) -> double {
        double mx = std::max(std::abs(a), std::abs(b));
        if (eckit::types::is_approximately_equal(mx, 0.)) {
            mx = 1.;
        }
        return std::abs(a - b) / mx;
    };

    if (doRelativeCompare_) {
        double error = relative_error(maxIndexValue1_, maxIndexValue2_);
        if (error > relativeErrorFactor_) {
            reasons << "\n" "* relative error (" << error << ") greater than " << relativeErrorFactor_;
        }
    }

    if (doRelativeMinCompare_) {
        double error = relative_error(counter1_.min(), counter2_.min());
        if (error > relativeErrorMin_) {
            reasons << "\n" "* minimum relative error (" << error << ") greater than " << relativeErrorMin_;
        }
    }

    if (doRelativeMaxCompare_) {
        double error = relative_error(counter1_.max(), counter2_.max());
        if (error > relativeErrorMax_) {
            reasons << "\n" "* maximum relative error (" << error << ") greater than " << relativeErrorMax_;
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


size_t CounterBinary::missingIn1NotIn2() const {
    return missingIn1NotIn2_;
}


size_t CounterBinary::missingIn2NotIn1() const {
    return missingIn1NotIn2_;
}


}  // namespace detail
}  // namespace stats
}  // namespace mir
