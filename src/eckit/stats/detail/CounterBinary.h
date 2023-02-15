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


#pragma once

#include "mir/stats/detail/Counter.h"


namespace mir::stats::detail {


/// Counter accounting for missing values, for differences of two MIRFIelds
struct CounterBinary {
private:
    Counter counter1_;
    Counter counter2_;

    size_t missing_;
    size_t missingIn1NotIn2_;
    size_t missingIn2NotIn1_;
    size_t maxIndex_;
    size_t countBelowLowerLimit_;
    size_t countAboveUpperLimit_;

    double max_;
    double lowerLimit_;
    double upperLimit_;
    double maxIndexValue1_;
    double maxIndexValue2_;

    double absoluteError_;
    double packingError_;
    double relativeErrorMin_;
    double relativeErrorMax_;
    double relativeErrorFactor_;

    size_t ignoreDifferentMissingValues_;
    size_t ignoreAboveUpperLimit_;
    double ignoreDifferentMissingValuesFactor_;
    double ignoreAboveUpperLimitFactor_;

    bool doAbsoluteCompare_;
    bool doPackingCompare_;
    bool doRelativeCompare_;
    bool doRelativeMinCompare_;
    bool doRelativeMaxCompare_;

    bool hasLowerLimit_;
    bool hasUpperLimit_;
    bool first_;

public:
    CounterBinary(const param::MIRParametrisation&, const param::MIRParametrisation&);

    void reset(const data::MIRField&, const data::MIRField&);
    void print(std::ostream&) const;
    bool count(double a, double b, double diff);
    std::string check() const;

    size_t ignoreAboveUpperLimit() const;
    size_t ignoreDifferentMissingValues() const;

    size_t count() const;
    size_t countAboveUpperLimit() const;
    size_t countBelowLowerLimit() const;

    double max() const;
    size_t maxIndex() const;

    size_t missing() const;
    size_t missingIn1NotIn2() const;
    size_t missingIn2NotIn1() const;
};


}  // namespace mir::stats::detail
