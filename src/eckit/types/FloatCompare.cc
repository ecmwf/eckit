#include <math.h>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

namespace eckit {

namespace detail {

template <class T>
inline T abs(T);

template<>
double abs(double x) { return fabs(x); }

template<>
float abs(float x) { return fabsf(x); }

union Double {
    typedef double float_t;
    typedef int64_t int_t;

    float_t f_;
    int_t i_;

    Double(double x) : f_(x) {}
};

union Float {
    typedef float float_t;
    typedef int32_t int_t;

    float_t f_;
    int_t i_;

    Float(float x) : f_(x) {}
};

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
template< typename T >
bool almostEqualUlps(T a, T b, T epsilon, int maxUlpsDiff) {

    // Bit identical is equal for any epsilon
    if (a == b) return true;

    // NaNs and infinity are always different
    if (isnan(a) || isnan(b) || isinf(a) || isinf(b)) return false;

    // Subnormal numbers are treated as 0
    if (fpclassify(a) == FP_SUBNORMAL) a = 0;
    if (fpclassify(b) == FP_SUBNORMAL) b = 0;

    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    if (detail::abs(a - b) <= epsilon) return true;

    // If either is zero, compare the absolute value of the other to the minimum normal number
    if (a == 0) {
        return (1 + detail::float_distance(detail::abs(b), std::numeric_limits<T>::min())) <= maxUlpsDiff;
    }
    if (b == 0) {
        return (1 + detail::float_distance(detail::abs(a), std::numeric_limits<T>::min())) <= maxUlpsDiff;
    }

    if (signbit(a) == signbit(b)) return detail::float_distance(a, b) <= maxUlpsDiff;

    // If signs are different, add ULP distances from minimum normal number on both sides of 0
    return (2 + detail::float_distance(a > 0 ? a : b, std::numeric_limits<T>::min())
              + detail::float_distance(a < 0 ? a : b, -std::numeric_limits<T>::min())) <= maxUlpsDiff;
}

template<>
bool isApproxEqualUlps(float a, float b, float epsilon, int maxUlpsDiff) {
    return almostEqualUlps(a, b, epsilon, maxUlpsDiff);
}

template<>
bool isApproxEqualUlps(double a, double b, double epsilon, int maxUlpsDiff) {
    return almostEqualUlps(a, b, epsilon, maxUlpsDiff);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
