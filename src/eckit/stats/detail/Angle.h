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


#ifndef mir_stats_detail_Angle_H
#define mir_stats_detail_Angle_H

#include <complex>  // defines std::pow(std::complex) used in ScalarCentralMomentsFn
#include "mir/stats/detail/CountMissingValuesFn.h"
#include "mir/stats/detail/ScalarCentralMomentsFn.h"
#include "mir/method/decompose/DecomposeToCartesian.h"


namespace mir {
namespace stats {
namespace detail {


/**
 * Angles statistics unary operator functor, suitable for angles in [r] or [°]
 */
struct Angle : CountMissingValuesFn<double> {
private:
    typedef CountMissingValuesFn<double> missing_t;
    typedef method::decompose::DecomposeToCartesian decompose_t;

    const decompose_t& decompose_;
    ScalarCentralMomentsFn< std::complex<double> > calculateCentralMoments_;

public:

    Angle(const method::decompose::DecomposeToCartesian& decompose, const double& missingValue) :
        decompose_(decompose) {
        reset(missingValue);
    }

    void reset(double missingValue=std::numeric_limits<double>::quiet_NaN()) {
        decompose_.setMissingValue(missingValue);
        missing_t::reset(missingValue);
        calculateCentralMoments_.reset();
    }

    double mean()              const { return decompose_.recomposeValue(calculateCentralMoments_.mean()); }
    double variance()          const { return decompose_.recomposeValue(calculateCentralMoments_.variance()); }
    double standardDeviation() const { return std::sqrt(std::abs(variance()));; }

    // skewness and kurtosis have no units (they're pure numbers)
    std::complex<double> skewness() const { return calculateCentralMoments_.skewness(); }  // works, but is it meaningful for circular quantities?
    std::complex<double> kurtosis() const { return calculateCentralMoments_.kurtosis(); }  // works, but is it meaningful for circular quantities?

    bool operator()(const double& v) {
        return missing_t::operator()(v)
                && calculateCentralMoments_(decompose_.decomposeValue(v));
    }

    bool operator()(const double& v1, const double& v2) {
        return missing_t::operator()(v1, v2)
                && calculateCentralMoments_(decompose_.decomposeValue(std::abs(v1 - v2)));
    }

    bool operator+=(const Angle& other) {
        missing_t::operator+=(other);
        calculateCentralMoments_ += other.calculateCentralMoments_;
        return true;
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
