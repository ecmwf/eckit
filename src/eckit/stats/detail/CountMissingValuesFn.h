/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#ifndef mir_stats_detail_CountMissingValuesFn_h
#define mir_stats_detail_CountMissingValuesFn_h

#include <cstddef>
#include "mir/util/Compare.h"


namespace mir {
namespace stats {
namespace detail {


/**
 * Counter binary operator functor: missing values
 * @note returns if values are good for comparison (ie. neither is missing)
 */
template< typename T >
struct CountMissingValuesFn : util::compare::ACompareBinFn<T> {
protected:
    util::compare::IsMissingFn isMissing1_;
    util::compare::IsMissingFn isMissing2_;
    size_t countMissing1_;
    size_t countMissing2_;
    size_t countDifferencesMissing_;
    size_t countNonMissing_;
    size_t countTotal_;

public:

    CountMissingValuesFn(
            const double& missingValue1=std::numeric_limits<double>::quiet_NaN(),
            const double& missingValue2=std::numeric_limits<double>::quiet_NaN() ) {
        reset(missingValue1, missingValue2);
    }

    void reset(
            const double& missingValue1=std::numeric_limits<double>::quiet_NaN(),
            const double& missingValue2=std::numeric_limits<double>::quiet_NaN() ) {
        isMissing1_.reset(missingValue1);
        isMissing2_.reset(missingValue2);
        countMissing1_   = 0;
        countMissing2_   = 0;
        countDifferencesMissing_ = 0;
        countNonMissing_ = 0;
        countTotal_      = 0;
    }

    size_t count()        const { return countNonMissing(); }
    size_t countMissing() const { return countMissing1(); }

    size_t countMissing1()   const { return countMissing1_; }
    size_t countMissing2()   const { return countMissing2_; }
    size_t countDifferencesMissing() const { return countDifferencesMissing_; }
    size_t countNonMissing() const { return countNonMissing_; }
    size_t countTotal()      const { return countTotal_; }

    /// @returns if value is good for comparison (ie. is not missing)
    bool operator()(const T& v) {
        ++countTotal_;
        if (isMissing1_(v)) {
            ++countMissing1_;
            return false;
        }
        ++countNonMissing_;
        return true;
    }

    /// @returns if value is good for comparison (ie. both are not missing)
    bool operator()(const T& v1, const T& v2) {
        ++countTotal_;
        if (!isMissing1_(v1) && !isMissing1_(v2)) {
            ++countNonMissing_;
            return true;
        }
        if (isMissing1_(v1) != isMissing1_(v2)) {
            ++countDifferencesMissing_;
        }
        if (isMissing1_(v1)) { ++countMissing1_; }
        if (isMissing2_(v2)) { ++countMissing2_; }
        return false;
    }

    bool operator+=(const CountMissingValuesFn& other) {
        countMissing1_           += other.countMissing1_;
        countMissing2_           += other.countMissing2_;
        countDifferencesMissing_ += other.countDifferencesMissing_;
        countNonMissing_         += other.countNonMissing_;
        countTotal_              += other.countTotal_;
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
