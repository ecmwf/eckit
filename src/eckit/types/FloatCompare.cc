//#include <cmath>

// Some of the math.h/cmath functions are not clean when switching to C++11
#if __cplusplus <= 199711L
#include <math.h>
#else
#include <cmath>
#define fpclassify(x) std::fpclassify((x))
#define isinf(x) std::isinf((x))
#define isnan(x) std::isnan((x))
#define signbit(x) std::signbit((x))
#endif

#include <sys/types.h>
#include <limits>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

namespace eckit {
namespace types {

//----------------------------------------------------------------------------------------------------------------------

namespace detail {

// FIXME: The following functions are available in std:: as of C++11:
// * fpclassify
// * isinf
// * isnan
// * signbit
// For the moment we have to use the (non namespaced) versions from math.h

template <class T>
inline T abs(T);

template <>
double abs(double x) {
    return fabs(x);
}

template <>
float abs(float x) {
    return fabsf(x);
}

// Used for accessing the integer representation of floating-point numbers
// (aliasing through unions works on most platforms).
union Double {
    typedef double float_t;
    typedef int64_t int_t;

    float_t f_;
    int_t i_;

    Double(double x) :
        f_(x) {}
};

// Used for accessing the integer representation of floating-point numbers
// (aliasing through unions works on most platforms).
union Float {
    typedef float float_t;
    typedef int32_t int_t;

    float_t f_;
    int_t i_;

    Float(float x) :
        f_(x) {}
};

// The difference between the bit representations of two floating point
// numbers of the same sign interpreted as a signed integer is equal to their
// distance from each other i.e. how many representable floating point numbers
// they are "apart". This is a very fast way to compute this difference.

Double::int_t float_distance(double x, double y) {
    const Double::int_t dist = Double(x).i_ - Double(y).i_;
    return dist >= 0 ? dist : -dist;
}

Float::int_t float_distance(float x, float y) {
    const Float::int_t dist = Float(x).i_ - Float(y).i_;
    return dist >= 0 ? dist : -dist;
}

}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

/// Compare 2 floats with an absolute epsilon check (values near zero), then based on ULPs
///
/// a, b       : two floating-point numbers (single/double precision) to compare.
/// epsilon    : epsilon for floating-point absolute epsilon check (should be some
///              small multiple of std::numeric_limits<T>::epsilon().
/// maxUlpsDiff: unit in the last place or unit of least precision.
///              maxUlpsDiff can also be interpreted in terms of how many representable floats we are
///              willing to accept between a and b. This function will allow maxUlpsDiff-1 floats
///              between a and b.
///
/// If two numbers are identical except for a one-bit difference in the last digit of their mantissa
/// then this function will calculate ulpsDiff as one.
/// If one number is the maximum number for a particular exponent, say 1.99999988,
/// and the other number is the smallest number for the next exponent, say 2.0,
/// then this function will again calculate ulpsDiff as one.
/// In both cases the two numbers are the closest floats there are.
///
/// Assumptions:
///   * Bit identical numbers are equal for any epsilon.
///   * A NaN is different from any number (even another NaN).
///   * Infinity is different from any number but infinity (with the same sign).
///   * Subnormal numbers are treated as equal to 0
///   * +/-std::numeric_limits<T>::min() has ULP distance 1 from 0
///   * -std::numeric_limits<T>::min() has ULP distance 2 from std::numeric_limits<T>::min()
///   * ULP distance from 0 is 1 + ULP distance from std::numeric_limits<T>::min() (for positive numbers)
///
template <typename T>
bool is_approximately_equal(T a, T b, T epsilon, int maxUlpsDiff) {

    // Bit identical is equal for any epsilon
    if (a == b)
        return true;

    // NaNs and infinity are always different
    if (isnan(a) || isnan(b) || isinf(a) || isinf(b))
        return false;

    // Subnormal numbers are treated as 0
    if (fpclassify(a) == FP_SUBNORMAL)
        a = 0;
    if (fpclassify(b) == FP_SUBNORMAL)
        b = 0;

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    if (detail::abs(a - b) <= epsilon)
        return true;

    // If either is zero, compare the absolute value of the other to the minimum normal number
    if (a == 0) {
        return (1 + detail::float_distance(detail::abs(b), std::numeric_limits<T>::min())) <= maxUlpsDiff;
    }
    if (b == 0) {
        return (1 + detail::float_distance(detail::abs(a), std::numeric_limits<T>::min())) <= maxUlpsDiff;
    }

    if (signbit(a) == signbit(b))
        return detail::float_distance(a, b) <= maxUlpsDiff;

    // If signs are different, add ULP distances from minimum normal number on both sides of 0
    const int64_t dp = detail::float_distance(a > 0 ? a : b, std::numeric_limits<T>::min());
    const int64_t dn = detail::float_distance(a < 0 ? a : b, -std::numeric_limits<T>::min());
    // Protect against integer overlow in case float distance for both numbers is close to maxint
    return dp < maxUlpsDiff && dn < maxUlpsDiff && (2 + dp + dn) <= maxUlpsDiff;
}

// Explicit template instantiaton
template bool is_approximately_equal(float a, float b, float epsilon, int maxUlpsDiff);
template bool is_approximately_equal(double a, double b, double epsilon, int maxUlpsDiff);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace types
}  // namespace eckit
