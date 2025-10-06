/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @author Florian Rathgeber
/// @date Jun 2015

#ifndef eckit_types_FloatCompare_h
#define eckit_types_FloatCompare_h

#include <limits>

namespace eckit::types {

//----------------------------------------------------------------------------------------------------------------------

/// Compare 2 floats with an absolute epsilon check (values near zero), then based on ULPs
///
/// epsilon    : epsilon for floating-point absolute epsilon check (should be some
///              small multiple of std::numeric_limits<T>::epsilon()).
///              Used to check values near zero
/// maxUlpsDiff: maximum spacing between the floating-point numbers A and B
///              unit in the last place or unit of least precision. defaults to 10.
///              maxUlpsDiff can also be interpreted in terms of how many representable floats we are
///              willing to accept between A and B. This function will allow maxUlpsDiff-1 floats
///              between A and B.
///
/// Assumptions:
///   * Bit identical numbers are equal for any epsilon.
///   * A NaN is different from any number (even another NaN).
///   * Infinity is different from any number but infinity (with the same sign).
///   * Subnormal numbers are treated as equal to 0
///   * +/-std::numeric_limits<T>::min() has ULP distance 1 from 0
///   * -std::numeric_limits<T>::min() has ULP distance 2 from std::numeric_limits<T>::min()
///   * ULP distance from 0 is 1 + ULP distance from std::numeric_limits<T>::min() (for positive numbers)

/// Compares values equality within a range defined by an epsilon and a
/// This function only has specializations for double and float
template <typename T>
bool is_approximately_equal(T a, T b, T epsilon = std::numeric_limits<T>::epsilon(), int maxUlpsDiff = 10);

/// Compare values equality
template <typename T>
bool is_equal(const T& a, const T& b) {
    return (a == b);
}

/// Compare values inequality: "is greater or equal to"
template <typename T>
bool is_greater_or_equal(const T& a, const T& b) {
    return (a >= b);
}

/// Compare values inequality: "is greater or approximately equal to"
template <typename T>
bool is_approximately_greater_or_equal(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon(),
                                       int maxUlpsDiff = 10) {
    return (a >= b) || eckit::types::is_approximately_equal(a, b, epsilon, maxUlpsDiff);
}

/// Compare values inequality: "is less or approximately equal to"
template <typename T>
bool is_approximately_lesser_or_equal(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon(),
                                      int maxUlpsDiff = 10) {
    return (a <= b) || eckit::types::is_approximately_equal(a, b, epsilon, maxUlpsDiff);
}

/// Compare values inequality: "is strictly greater than"
template <typename T>
bool is_strictly_greater(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon(), int maxUlpsDiff = 10) {
    return (a > b) && !eckit::types::is_approximately_equal(a, b, epsilon, maxUlpsDiff);
}

//----------------------------------------------------------------------------------------------------------------------

/// Helper class that memorizes the comparison setttings
template <typename T>
class CompareApproximatelyEqual {

    T eps_;
    int maxUlps_;

public:

    CompareApproximatelyEqual(T eps = std::numeric_limits<T>::epsilon(), int maxUlps = 10) :
        eps_(eps), maxUlps_(maxUlps) {}

    bool operator()(const T& a, const T& b) { return eckit::types::is_approximately_equal(a, b, eps_, maxUlps_); }
};

//----------------------------------------------------------------------------------------------------------------------

// Forward declaration of explicit template instatiations, to avoid warnings
extern template bool is_approximately_equal(float a, float b, float epsilon, int maxUlpsDiff);
extern template bool is_approximately_equal(double a, double b, double epsilon, int maxUlpsDiff);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::types

#endif
