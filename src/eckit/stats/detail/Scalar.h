/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#ifndef mir_stats_detail_Scalar_h
#define mir_stats_detail_Scalar_h

#include "mir/stats/detail/CountMissingValuesFn.h"
#include "mir/stats/detail/ScalarCentralMomentsFn.h"
#include "mir/stats/detail/ScalarMinMaxFn.h"
#include "mir/stats/detail/ScalarpNormsFn.h"


namespace mir {
namespace stats {
namespace detail {


/**
 * Statistics unary operator functor: composition of above functionality (suitable for scalars)
 */
template< typename T >
struct Scalar : CountMissingValuesFn<T> {
private:
    ScalarMinMaxFn<T>         calculateMinMax_;
    ScalarCentralMomentsFn<T> calculateCentralMoments_;
    ScalarpNormsFn<T>         calculateNorms_;
    typedef CountMissingValuesFn<T> missing_t;
    //NOTE: not using difference mode for the moment (2nd argument to missing_t)

public:

    Scalar(const double& missingValue=std::numeric_limits<double>::quiet_NaN()) {
        reset(missingValue);
    }

    void reset(double missingValue=std::numeric_limits<double>::quiet_NaN()) {
        missing_t::reset(missingValue);
        calculateMinMax_.reset();
        calculateCentralMoments_.reset();
        calculateNorms_.reset();
    }

    T min()               const { return calculateMinMax_.min(); }
    T max()               const { return calculateMinMax_.max(); }
    size_t minIndex()     const { return calculateMinMax_.minIndex(); }
    size_t maxIndex()     const { return calculateMinMax_.maxIndex(); }

    T mean()              const { return calculateCentralMoments_.mean(); }
    T variance()          const { return calculateCentralMoments_.variance(); }
    T standardDeviation() const { return calculateCentralMoments_.standardDeviation(); }
    T skewness()          const { return calculateCentralMoments_.skewness(); }
    T kurtosis()          const { return calculateCentralMoments_.kurtosis(); }

    T normL1()            const { return calculateNorms_.normL1(); }
    T normL2()            const { return calculateNorms_.normL2(); }
    T normLinfinity()     const { return calculateNorms_.normLinfinity(); }

    bool operator()(const T& v) {
        return missing_t::operator()(v)
                && calculateMinMax_        (v)
                && calculateCentralMoments_(v)
                && calculateNorms_         (v);
    }

    bool operator()(const T& v1, const T& v2) {
        if (missing_t::operator()(v1, v2)) {
            // if value is good for comparison
            const T v = std::abs(v1 - v2);
            return calculateMinMax_        (v)
                && calculateCentralMoments_(v)
                && calculateNorms_         (v);
        }
        return false;
    }

    bool operator+=(const Scalar& other) {
        missing_t::operator+=(other);
        calculateMinMax_         += other.calculateMinMax_;
        calculateCentralMoments_ += other.calculateCentralMoments_;
        calculateNorms_          += other.calculateNorms_;
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
