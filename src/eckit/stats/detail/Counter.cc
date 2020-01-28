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


#include "mir/stats/detail/Counter.h"

#include <limits>
#include <ostream>

#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace stats {
namespace detail {


Counter::Counter(const param::MIRParametrisation& parametrisation) :
    count_(0),
    missing_(0),
    countBelowLowerLimit_(0),
    countAboveUpperLimit_(0),
    minIndex_(0),
    maxIndex_(0),
    lowerLimit_(std::numeric_limits<double>::quiet_NaN()),
    upperLimit_(std::numeric_limits<double>::quiet_NaN()),
    min_(std::numeric_limits<double>::quiet_NaN()),
    max_(std::numeric_limits<double>::quiet_NaN()),
    first_(true) {

    parametrisation.get("counter-lower-limit", lowerLimit_);
    parametrisation.get("counter-upper-limit", upperLimit_);
    hasLowerLimit_ = lowerLimit_ == lowerLimit_;
    hasUpperLimit_ = upperLimit_ == upperLimit_;
}


void Counter::reset(const data::MIRField& field) {
    ASSERT(field.dimensions() == 1);

    count_   = 0;
    missing_ = 0;
    countBelowLowerLimit_ = 0;
    countAboveUpperLimit_ = 0;

    missingValue_ = field.missingValue();
    hasMissing_ = field.hasMissing();

    minIndex_ = 0;
    maxIndex_ = 0;
    min_ = std::numeric_limits<double>::quiet_NaN();
    max_ = std::numeric_limits<double>::quiet_NaN();
    first_ = true;
}


void Counter::print(std::ostream& out) const {
    out << "Counter["
            "count=" << count()
        << ",missing=" << missing();
    if (hasUpperLimit_) {
        out << ",countAboveUpperLimit=" << countAboveUpperLimit();
    }
    if (hasLowerLimit_) {
        out << ",countBelowLowerLimit=" << countBelowLowerLimit();
    }
    out << ",max="      << max()
        << ",maxIndex=" << maxIndex()
        << ",min="      << min()
        << ",minIndex=" << minIndex()
        << "]";
}


bool Counter::count(const double &v) {

    if (hasMissing_ && missingValue_ == v) {
        count_++;
        missing_++;
        return false;
    }

    if (hasLowerLimit_ && v < lowerLimit_) {
        ++countBelowLowerLimit_;
    }

    if (hasUpperLimit_ && v > upperLimit_) {
        ++countAboveUpperLimit_;
    }

    if (min_ > v || first_) {
        min_ = v;
        minIndex_ = count_;
    }

    if (max_ < v || first_) {
        max_ = v;
        maxIndex_ = count_;
    }

    count_++;
    first_ = false;
    return true;
}


size_t Counter::count() const {
    return count_;
}


size_t Counter::missing() const {
    return missing_;
}


size_t Counter::countBelowLowerLimit() const {
    return countBelowLowerLimit_;
}


size_t Counter::countAboveUpperLimit() const {
    return countAboveUpperLimit_;
}


double Counter::min() const {
    return min_;
}


size_t Counter::minIndex() const {
    return minIndex_;
}


double Counter::max() const {
    return max_;
}


size_t Counter::maxIndex() const {
    return maxIndex_;
}


}  // namespace detail
}  // namespace stats
}  // namespace mir

