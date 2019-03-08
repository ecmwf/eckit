/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_MinMax_h
#define mir_stats_detail_MinMax_h

#include <cstddef>
#include <limits>
#include <ostream>

#include "eckit/parser/JSON.h"


namespace mir {
namespace stats {
namespace detail {


/// Min/max range statistics
struct MinMax {
private:
    double min_;
    double max_;
    size_t minIndex_;
    size_t maxIndex_;
    size_t count_;

public:

    MinMax() {
        reset();
    }

    void reset() {
        min_ = std::numeric_limits<double>::quiet_NaN();
        max_ = std::numeric_limits<double>::quiet_NaN();
        minIndex_ = 0;
        maxIndex_ = 0;
        count_    = 0;
    }

    double min() const { return min_; }
    double max() const { return max_; }
    size_t minIndex() const { return minIndex_; }
    size_t maxIndex() const { return maxIndex_; }
    size_t count()    const { return count_; }

    void operator()(const double& v) {
        if (min_!=min_ || min_>v) {
            min_ = v;
            minIndex_ = count_;
        }
        if (max_!=max_ || max_<v) {
            max_ = v;
            maxIndex_ = count_;
        }
        count_++;
    }

    void operator+=(const MinMax& other) {
        if (other.count_) {
            if (min_!=min_ || min_>other.min_) {
                min_ = other.min_;
                minIndex_ = other.minIndex_;
            }
            if (max_!=max_ || max_<other.max_) {
                max_ = other.max_;
                maxIndex_ = other.maxIndex_;
            }
        }
    }

    void print(std::ostream& out) const {
        out << "MinMax[";
        eckit::JSON j(out);
        j.startObject()
                << "min" << min()
                << "max" << max()
                << "minindex" << minIndex()
                << "maxindex" << maxIndex();
        j.endObject();
        out << "]";
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
