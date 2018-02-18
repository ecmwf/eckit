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


#ifndef mir_stats_detail_ScalarpNormsFn_h
#define mir_stats_detail_ScalarpNormsFn_h

#include <cmath>

namespace mir {
namespace stats {
namespace detail {


/**
 * Statistics unary operator functor: p-norm (p=1, p=2 and p=infinity)
 * @see https://en.wikipedia.org/wiki/Lp_space
 * @see https://en.wikipedia.org/wiki/Minkowski_distance
 */
template< typename T >
struct ScalarpNormsFn {
private:
    T normL1_;
    T sumSquares_;
    T normLinfinity_;

public:

    ScalarpNormsFn() {
        reset();
    }

    void reset() {
        normL1_        = 0;
        sumSquares_    = 0;
        normLinfinity_ = 0;
    }

    T normL1()        const { return normL1_; }
    T normL2()        const { return std::sqrt(sumSquares_); }
    T normLinfinity() const { return normLinfinity_; }

    bool operator()(const T& v) {
        normL1_       += std::abs(v);
        sumSquares_   += v*v;
        normLinfinity_ = std::max(normLinfinity_, std::abs(v));
        return true;
    }

    bool operator+=(const ScalarpNormsFn& other) {
        normL1_       += other.normL1_;
        sumSquares_   += other.sumSquares_;
        normLinfinity_ = std::max(normLinfinity_, other.normLinfinity_);
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
