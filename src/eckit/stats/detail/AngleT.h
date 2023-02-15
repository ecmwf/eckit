/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <cmath>
#include <complex>
#include <limits>
#include <ostream>

#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/util/Exceptions.h"


namespace mir::stats::detail {


enum AngleScale
{
    DEGREE,
    RADIAN
};
enum AngleSpace
{
    ASYMMETRIC,
    SYMMETRIC
};


/// Angle statistics in degrees/radians or symmetric/non-symmetric
template <typename T, int SCALE, int SYMMETRY>
struct AngleT {
private:
    CentralMomentsT<std::complex<T>> centralMoments_;

    const T rescale_;
    const T descale_;
    const T globe_;
    const T min_;

    T normalise(T a, T minimum, T globe) const {
        while (a >= minimum + globe) {
            a -= globe;
        }
        while (a < minimum) {
            a += globe;
        }
        return a;
    }

    T normalise(T a) const { return normalise(a, min_, globe_); }

    std::complex<T> decompose(const T& a) const { return std::polar(1., a * descale_); }

    T recompose(const std::complex<T>& c) const { return normalise(std::arg(c) * rescale_); }

public:
    AngleT() :
        rescale_(std::numeric_limits<T>::signaling_NaN()),
        descale_(std::numeric_limits<T>::signaling_NaN()),
        globe_(std::numeric_limits<T>::signaling_NaN()),
        min_(std::numeric_limits<T>::signaling_NaN()) {
        NOTIMP;  // ensure specialisation
    }

    void reset() { centralMoments_.reset(); }

    T difference(const T& a, const T& b) const {
        auto d = std::abs(normalise(b) - normalise(a));
        return std::min(globe_ - d, d);
    }

    void operator()(const T& v) { centralMoments_(decompose(v)); }

    void operator+=(const AngleT& other) { centralMoments_ += other.centralMoments_; }

    T mean() const { return recompose(centralMoments_.mean()); }

    T variance() const { return recompose(centralMoments_.variance()); }

    T standardDeviation() const { return recompose(centralMoments_.standardDeviation()); }

    void print(std::ostream& out) const {
        out << "Angle["
               "mean="
            << mean() << ",stddev=" << standardDeviation() << "]";
    }
};


// Available angle statistics
template <>
AngleT<double, AngleScale::DEGREE, AngleSpace::ASYMMETRIC>::AngleT::AngleT();
template <>
AngleT<double, AngleScale::DEGREE, AngleSpace::SYMMETRIC>::AngleT::AngleT();
template <>
AngleT<double, AngleScale::RADIAN, AngleSpace::ASYMMETRIC>::AngleT::AngleT();
template <>
AngleT<double, AngleScale::RADIAN, AngleSpace::SYMMETRIC>::AngleT::AngleT();


}  // namespace mir::stats::detail
