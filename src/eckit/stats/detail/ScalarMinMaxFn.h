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


#ifndef mir_stats_detail_ScalarMinMaxFn_h
#define mir_stats_detail_ScalarMinMaxFn_h


namespace mir {
namespace stats {
namespace detail {


/**
 * Statistics unary operator functor: min/max range
 * @note: operator+= (the pairwise version) invalidates minIndex() and maxIndex() because they are local to the functor
 */
template< typename T >
struct ScalarMinMaxFn {
private:
    T min_;
    T max_;
    size_t minIndex_;
    size_t maxIndex_;
    size_t count_;

public:

    ScalarMinMaxFn() { reset(); }

    void reset() {
        min_ = std::numeric_limits<T>::quiet_NaN();
        max_ = std::numeric_limits<T>::quiet_NaN();
        minIndex_ = 0;
        maxIndex_ = 0;
        count_ = 0;
    }

    T min() const { return min_; }
    T max() const { return max_; }
    size_t minIndex() const { return minIndex_; }
    size_t maxIndex() const { return maxIndex_; }

    bool operator()(const T& v) {
        ++count_;
        if (min_!=min_ || min_>v) { min_ = v; minIndex_ = count_; }
        if (max_!=max_ || max_<v) { max_ = v; maxIndex_ = count_; }
        return true;
    }

    bool operator+=(const ScalarMinMaxFn& other) {
        if (other.count_) {
            if (min_!=min_ || min_>other.min_) { min_ = other.min_; minIndex_ = 0; }
            if (max_!=max_ || max_<other.max_) { max_ = other.max_; maxIndex_ = 0; }
        }
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
