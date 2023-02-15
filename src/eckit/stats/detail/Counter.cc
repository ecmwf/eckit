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
#include "mir/util/Exceptions.h"


namespace mir::stats::detail {


static double get_param(const param::MIRParametrisation& param, const std::string& key, double def) {
    double value(def);
    param.get(key, value);
    return value;
}


Counter::Counter(double missingValue, bool hasMissing, double lowerLimit, double upperLimit) :
    count_(0),
    missing_(0),
    countBelowLowerLimit_(0),
    countAboveUpperLimit_(0),
    minIndex_(0),
    maxIndex_(0),
    missingValue_(missingValue),
    lowerLimit_(lowerLimit),
    upperLimit_(upperLimit),
    min_(std::numeric_limits<double>::quiet_NaN()),
    max_(std::numeric_limits<double>::quiet_NaN()),
    hasMissing_(hasMissing),
    hasLowerLimit_(lowerLimit_ == lowerLimit_),
    hasUpperLimit_(upperLimit_ == upperLimit_),
    first_(true) {}


Counter::Counter(const param::MIRParametrisation& parametrisation) :
    Counter(std::numeric_limits<double>::quiet_NaN(), false,
            get_param(parametrisation, "counter-lower-limit", std::numeric_limits<double>::quiet_NaN()),
            get_param(parametrisation, "counter-upper-limit", std::numeric_limits<double>::quiet_NaN())) {}


void Counter::reset(double missingValue, bool hasMissing) {
    count_                = 0;
    missing_              = 0;
    countBelowLowerLimit_ = 0;
    countAboveUpperLimit_ = 0;

    missingValue_ = missingValue;
    hasMissing_   = hasMissing;

    minIndex_ = 0;
    maxIndex_ = 0;
    min_      = std::numeric_limits<double>::quiet_NaN();
    max_      = std::numeric_limits<double>::quiet_NaN();
    first_    = true;
}


void Counter::reset(const data::MIRField& field) {
    ASSERT(field.dimensions() == 1);
    reset(field.missingValue(), field.hasMissing());
}


void Counter::print(std::ostream& out) const {
    out << "Counter["
           "count="
        << count() << ",missing=" << missing();
    if (hasUpperLimit_) {
        out << ",countAboveUpperLimit=" << countAboveUpperLimit();
    }
    if (hasLowerLimit_) {
        out << ",countBelowLowerLimit=" << countBelowLowerLimit();
    }
    out << ",max=" << max() << ",maxIndex=" << maxIndex() << ",min=" << min() << ",minIndex=" << minIndex() << "]";
}


bool Counter::count(const double& v) {

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
        min_      = v;
        minIndex_ = count_;
    }

    if (max_ < v || first_) {
        max_      = v;
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


}  // namespace mir::stats::detail
