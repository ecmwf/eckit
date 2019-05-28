/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_detail_AngleT_h
#define mir_stats_detail_AngleT_h

#include <cmath>
#include <complex>
#include <limits>
#include <ostream>

#include "eckit/exception/Exceptions.h"

#include "mir/stats/detail/CentralMomentsT.h"


namespace mir {
namespace stats {
namespace detail {


enum AngleScale { DEGREE, RADIAN };
enum AngleSpace { ASYMMETRIC, SYMMETRIC };


/// Angle statistics in degrees [-180,180] or radians [-π,π]
template< int SCALE, int SYMMETRY >
struct AngleT {
private:
    CentralMomentsT<std::complex<double>> centralMoments_;

    const double rescale_;
    const double descale_;
    const double globe_;
    const double min_;

    double normalise(double a, double minimum, double globe) const {
        while (a >= minimum + globe) {
            a -= globe;
        }
        while (a < minimum) {
            a += globe;
        }
        return a;
    }

    double normalise(double a) const {
        return normalise(a, min_, globe_);
    }

    std::complex<double> decompose(const double& a) const {
        return std::polar(1., a * descale_);
    }

    double recompose(const std::complex<double>& c) const {
        return normalise(std::arg(c) * rescale_);
    }

public:

    AngleT() :
        rescale_(std::numeric_limits<double>::signaling_NaN()),
        descale_(std::numeric_limits<double>::signaling_NaN()),
        globe_(std::numeric_limits<double>::signaling_NaN()),
        min_(std::numeric_limits<double>::signaling_NaN()) {
        NOTIMP;  // ensure specialisation
    }

    void reset() {
        centralMoments_.reset();
    }

    double difference(const double& a, const double& b) const {
        auto d = std::abs(normalise(b) - normalise(a));
        return std::min(globe_ - d, d);
    }

    void operator()(const double& v) {
        centralMoments_(decompose(v));
    }

    void operator+=(const AngleT& other) {
        centralMoments_ += other.centralMoments_;
    }

    double mean() const {
        return recompose(centralMoments_.mean());
    }

    double variance() const {
        return recompose(centralMoments_.variance());
    }

    double standardDeviation() const {
        return recompose(centralMoments_.standardDeviation());
    }

    void print(std::ostream& out) const {
        out << "Angle["
                "mean="   << mean()
            << ",stddev=" << standardDeviation()
            << "]";
    }
};


// Available angle statistics
template<> AngleT<AngleScale::DEGREE, AngleSpace::ASYMMETRIC>::AngleT::AngleT();
template<> AngleT<AngleScale::DEGREE, AngleSpace::SYMMETRIC>::AngleT::AngleT();
template<> AngleT<AngleScale::RADIAN, AngleSpace::ASYMMETRIC>::AngleT::AngleT();
template<> AngleT<AngleScale::RADIAN, AngleSpace::SYMMETRIC>::AngleT::AngleT();


}  // namespace detail
}  // namespace stats
}  // namespace mir


#endif
