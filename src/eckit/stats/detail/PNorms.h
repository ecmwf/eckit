/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_PNorms_h
#define mir_stats_detail_PNorms_h

#include <algorithm>
#include <cmath>
#include <ostream>


namespace mir {
namespace stats {
namespace detail {


/**
 * Statistics unary operator functor: p-norm (p=1, p=2 and p=infinity)
 * @see https://en.wikipedia.org/wiki/Lp_space
 * @see https://en.wikipedia.org/wiki/Minkowski_distance
 */
struct PNorms {
private:
    double normL1_;
    double sumSquares_;
    double normLinfinity_;

public:

    PNorms() {
        reset();
    }

    void reset() {
        normL1_        = 0;
        sumSquares_    = 0;
        normLinfinity_ = 0;
    }

    double normL1()        const { return normL1_; }
    double normL2()        const { return std::sqrt(sumSquares_); }
    double normLinfinity() const { return normLinfinity_; }

    double difference(const double& a, const double& b) const {
        return std::abs(a - b);
    }

    void operator()(const double& v) {
        normL1_       += std::abs(v);
        sumSquares_   += v*v;
        normLinfinity_ = std::max(normLinfinity_, std::abs(v));
    }

    void operator+=(const PNorms& other) {
        normL1_       += other.normL1_;
        sumSquares_   += other.sumSquares_;
        normLinfinity_ = std::max(normLinfinity_, other.normLinfinity_);
    }

    void print(std::ostream& out) const {
        out << "PNorms["
                "L1=" << normL1()
            << ",L2=" << normL2()
            << ",Li=" << normLinfinity()
            << "]";
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
