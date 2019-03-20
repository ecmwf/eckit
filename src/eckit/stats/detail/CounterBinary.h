/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_CounterBinary_h
#define mir_stats_detail_CounterBinary_h

#include "mir/stats/detail/Counter.h"


namespace mir {
namespace stats {
namespace detail {


/// Counter accounting for missing values, for differences of two MIRFIelds
struct CounterBinary {
private:

    Counter counter1_;
    Counter counter2_;

    size_t missing_;
    size_t missingDifferent_;
    size_t maxIndex_;
    size_t countBelowLowerLimit_;
    size_t countAboveUpperLimit_;

    double max_;
    double lowerLimit_;
    double upperLimit_;

    double tolerance_;
    double ref;
    size_t ignoreDifferentMissingValues_;
    size_t ignoreCountAboveUpperLimit_;
    size_t ignoreDifferentMissingValuesFactor_;
    double ignoreCountAboveUpperLimitFactor_;

    enum tolerance_t {NONE, ABSOLUTE, RELATIVETOREFERENCE, RELATIVETOMAXIMUM, PACKINGERROR } toleranceType_;

    bool hasLowerLimit_;
    bool hasUpperLimit_;
    bool first_;

public:

    CounterBinary(const param::MIRParametrisation&, const param::MIRParametrisation&);

    void reset(const data::MIRField&, const data::MIRField&);
    void print(std::ostream&) const;
    bool count(const double&, const double&);
    bool check() const;

    double tolerance() const;
    size_t ignoreCountAboveUpperLimit() const;
    size_t ignoreDifferentMissingValues() const;

    size_t count() const;
    size_t countAboveUpperLimit() const;
    size_t countBelowLowerLimit() const;

    double max() const;
    size_t maxIndex() const;

    size_t missing() const;
    size_t missingDifferent() const;

};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
