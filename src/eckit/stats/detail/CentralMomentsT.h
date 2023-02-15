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
#include <cstddef>
#include <limits>
#include <ostream>


namespace mir::stats::detail {


/**
 * Statistics unary operator functor: standardized/central moments, up to 4th-order
 * @see Sandia Report SAND2008-6212 "Formulas for Robust, One-Pass Parallel Computation of Covariances and
 * Arbitrary-Order Statistical Moments," 2008 P. Pébay
 * @see http://people.xiph.org/~tterribe/notes/homs.html "Computing Higher-Order Moments Online," 2008 B. Terriberry
 * @note: variance is computed as an unbiased estimator (unbiased sample variance), s^2_{N-1} @see
 * http://mathworld.wolfram.com/Variance.html
 * @note: skewness (γ_1 = μ_3/μ_2^{3/2}) is computed as per @see http://mathworld.wolfram.com/Skewness.html
 * @note: kurtosis (γ_2 = μ_4/μ_2^2 - 3) is computed as kurtosis "excess", @see
 * http://mathworld.wolfram.com/Kurtosis.html
 */
template <typename T>
struct CentralMomentsT {
private:
    T M1_;
    T M2_;
    T M3_;
    T M4_;
    size_t count_;

public:
    CentralMomentsT() : count_(0) { reset(); }

    virtual ~CentralMomentsT() = default;

    void reset() {
        M1_    = 0;
        M2_    = 0;
        M3_    = 0;
        M4_    = 0;
        count_ = 0;
    }

    T centralMoment1() const { return count_ < 1 ? T(0) : T(M1_) / T(count_); }
    T centralMoment2() const { return count_ < 1 ? T(0) : T(M2_) / T(count_); }
    T centralMoment3() const { return count_ < 1 ? T(0) : T(M3_) / T(count_); }
    T centralMoment4() const { return count_ < 1 ? T(0) : T(M4_) / T(count_); }

    T mean() const { return count_ < 1 ? std::numeric_limits<T>::quiet_NaN() : M1_; }
    T variance() const { return count_ < 2 ? T(0) : T(M2_) / T(count_ - 1); }
    T skewness() const { return count_ < 2 ? T(0) : T(M3_) * std::sqrt(T(count_)) / std::pow(M2_, 1.5); }
    T kurtosis() const { return count_ < 2 ? T(0) : T(M4_) * T(count_) / T(M2_ * M2_) - 3.; }
    T standardDeviation() const { return std::sqrt(variance()); }

    T difference(const T& a, const T& b) const { return std::abs(a - b); }

    void operator()(const T& v) {
        const T n1   = T(count_);
        const T n    = T(count_ + 1);
        const T dx   = v - M1_;
        const T dx_n = dx / n;
        M4_ += -4. * dx_n * M3_ + 6. * dx_n * dx_n * M2_ + n1 * (n1 * n1 - n1 + 1.) * dx_n * dx_n * dx_n * dx;
        M3_ += -3. * dx_n * M2_ + n1 * (n1 - 1.) * dx_n * dx_n * dx;
        M2_ += n1 * dx_n * dx;
        M1_ += dx_n;

        count_++;
    }

    void operator+=(const CentralMomentsT& other) {
        if (other.count_) {
            const T n1   = T(count_);
            const T n2   = T(other.count_);
            const T n    = T(count_ + other.count_);
            const T dx   = other.M1_ - M1_;
            const T dx_n = dx / n;
            M4_ += other.M4_ + n1 * n2 * (n1 * n1 - n1 * n2 + n2 * n2) * dx_n * dx_n * dx_n * dx +
                   6. * (n1 * n1 * other.M2_ + n2 * n2 * M2_) * dx_n * dx_n + 4. * (n1 * other.M3_ - n2 * M3_) * dx_n;
            M3_ += other.M3_ + n1 * n2 * (n1 - n2) * dx_n * dx_n * dx + 3. * (n1 * other.M2_ - n2 * M2_) * dx_n;
            M2_ += other.M2_ + n1 * n2 * dx_n * dx;
            M1_ += n2 * dx_n;
        }

        count_ += other.count_;
    }

    virtual void print(std::ostream& out) const {
        out << "CentralMoments["
               "mean="
            << mean() << ",stddev=" << standardDeviation() << ",skewness=" << skewness() << ",kurtosis=" << kurtosis()
            << ",centralMoment1=" << centralMoment1() << ",centralMoment2=" << centralMoment2()
            << ",centralMoment3=" << centralMoment3() << ",centralMoment4=" << centralMoment4() << "]";
    }
};


}  // namespace mir::stats::detail
