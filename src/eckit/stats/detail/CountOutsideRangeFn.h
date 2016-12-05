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


#ifndef mir_stats_detail_CountOutsideRangeFn_h
#define mir_stats_detail_CountOutsideRangeFn_h

#include <limits>

namespace mir {
namespace stats {
namespace detail {


/**
 * Counter unary operator functor: outside range
 */
template< typename T >
struct CountOutsideRangeFn {
protected:
    const T lowerLimit_;
    const T upperLimit_;
    size_t count_;

public:

    CountOutsideRangeFn(
            const T& lowerLimit=std::numeric_limits<T>::quiet_NaN(),
            const T& upperLimit=std::numeric_limits<T>::quiet_NaN() ) :
        lowerLimit_(lowerLimit),
        upperLimit_(upperLimit) {
        reset();
    }

    void reset() {
        count_ = 0;
    }

    size_t count() const { return count_; }

    bool operator()(const T& v) {
        if (lowerLimit_==lowerLimit_ && v<lowerLimit_) { ++count_; }
        if (upperLimit_==upperLimit_ && v>upperLimit_) { ++count_; }
        return true;
    }

    bool operator+=(const CountOutsideRangeFn& other) {
        count_ += other.count_;
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
