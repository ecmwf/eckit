/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Oct 2016


#ifndef mir_stats_detail_MinMaxStatistics_h
#define mir_stats_detail_MinMaxStatistics_h

#include "mir/stats/detail/CountMissingValuesFn.h"
#include "mir/stats/detail/ScalarMinMaxFn.h"


namespace mir {
namespace stats {
namespace detail {


/**
 * Minimum-maximum statistics unary operator functor (suitable for scalars)
 */
template< typename T >
struct MinMaxStatistics : CountMissingValuesFn<T> {
private:
    ScalarMinMaxFn<T> calculateMinMax_;
    typedef CountMissingValuesFn<T> missing_t;
    //NOTE: not using difference mode for the moment (2nd argument to missing_t)

public:

    MinMaxStatistics(const double& missingValue=std::numeric_limits<double>::quiet_NaN()) {
        reset(missingValue);
    }

    void reset(double missingValue=std::numeric_limits<double>::quiet_NaN()) {
        missing_t::reset(missingValue);
        calculateMinMax_.reset();
    }

    T min()               const { return calculateMinMax_.min(); }
    T max()               const { return calculateMinMax_.max(); }
    size_t minIndex()     const { return calculateMinMax_.minIndex(); }
    size_t maxIndex()     const { return calculateMinMax_.maxIndex(); }

    bool operator()(const T& v) {
        return missing_t::operator()(v)
                && calculateMinMax_(v);
    }

    bool operator()(const T& v1, const T& v2) {
        if (missing_t::operator()(v1, v2)) {
            // if value is good for comparison
            const T v = std::abs(v1 - v2);
            return calculateMinMax_(v);
        }
        return false;
    }

    bool operator+=(const MinMaxStatistics& other) {
        missing_t::operator+=(other);
        calculateMinMax_ += other.calculateMinMax_;
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
