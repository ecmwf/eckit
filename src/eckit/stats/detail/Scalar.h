/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_Scalar_h
#define mir_stats_detail_Scalar_h

#include <ostream>

#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/MinMax.h"
#include "mir/stats/detail/PNorms.h"


namespace mir {
namespace stats {
namespace detail {


/// Scalar statistics (composition)
struct Scalar {
private:
    MinMax minMax_;
    PNorms pNorms_;
    CentralMomentsT<double> centralMoments_;

public:

    void reset() {
        minMax_.reset();
        centralMoments_.reset();
        pNorms_.reset();
    }

    double min()      const { return minMax_.min(); }
    double max()      const { return minMax_.max(); }
    size_t minIndex() const { return minMax_.minIndex(); }
    size_t maxIndex() const { return minMax_.maxIndex(); }

    double mean()              const { return centralMoments_.mean(); }
    double variance()          const { return centralMoments_.variance(); }
    double standardDeviation() const { return centralMoments_.standardDeviation(); }
    double skewness()          const { return centralMoments_.skewness(); }
    double kurtosis()          const { return centralMoments_.kurtosis(); }

    double normL1()        const { return pNorms_.normL1(); }
    double normL2()        const { return pNorms_.normL2(); }
    double normLinfinity() const { return pNorms_.normLinfinity(); }

    void operator()(const double& v) {
        minMax_        (v);
        centralMoments_(v);
        pNorms_         (v);
    }

    void operator+=(const Scalar& other) {
        minMax_         += other.minMax_;
        centralMoments_ += other.centralMoments_;
        pNorms_         += other.pNorms_;
    }

    void print(std::ostream& out) const {
        out << "Scalar[";
        minMax_.print(out);
        out << ",";
        centralMoments_.print(out);
        out << ",";
        pNorms_.print(out);
        out << "]";
    }
};


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
