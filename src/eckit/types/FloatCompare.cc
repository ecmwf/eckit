#include <math.h>
#include <limits>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// The following code is adapted from http://www.boost.org/doc/libs/1_62_0/boost/math/special_functions/next.hpp
//
//  (C) Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//----------------------------------------------------------------------------------------------------------------------

namespace detail {

template <class T>
inline int sign(const T& z) {
   return (z == 0) ? 0 : signbit(z) ? -1 : 1;
}

template <class T>
inline int digits() {
    return std::numeric_limits<T>::radix == 2
            ? std::numeric_limits<T>::digits
            : ((std::numeric_limits<T>::digits + 1) * 1000L) / 301L;
}

template <class T>
inline T abs(T);

template<>
double abs(double x) { return fabs(x); }

template<>
float abs(float x) { return fabsf(x); }

template <class T>
inline T frExp(T, int*);

template<>
double frExp(double x, int* e) { return frexp(x, e); }

template<>
float frExp(float x, int* e) { return frexpf(x, e); }

template <class T>
inline T ldExp(T, int);

template<>
double ldExp(double x, int e) { return ldexp(x, e); }

template<>
float ldExp(float x, int e) { return ldexpf(x, e); }

template <class T>
T float_distance(const T& a, const T& b) {
    //
    // Error handling:
    //
    if (!isfinite(a)) {
        std::ostringstream s;
        s << "First argument must be finite, but got " << a << std::endl;
        throw BadParameter(s.str()); // for efficiency don't use CodeLocation Here()
    }
    if (!isfinite(b)) {
        std::ostringstream s;
        s << "Second argument must be finite, but got " << b << std::endl;
        throw BadParameter(s.str()); // for efficiency don't use CodeLocation Here()
    }

    //
    // Special cases:
    //
    if(a > b)
        return -float_distance(b, a);
    if(a == b)
        return T(0);
    if(a == 0)
        return 1 + abs(float_distance(static_cast<T>((b < 0) ? T(-std::numeric_limits<T>::min()) : std::numeric_limits<T>::min()), b));
    if(b == 0)
        return 1 + abs(float_distance(static_cast<T>((a < 0) ? T(-std::numeric_limits<T>::min()) : std::numeric_limits<T>::min()), a));
    if(sign(a) != sign(b))
        return 2 + abs(float_distance(static_cast<T>((b < 0) ? T(-std::numeric_limits<T>::min()) : std::numeric_limits<T>::min()), b))
                 + abs(float_distance(static_cast<T>((a < 0) ? T(-std::numeric_limits<T>::min()) : std::numeric_limits<T>::min()), a));
    //
    // By the time we get here, both a and b must have the same sign, we want
    // b > a and both postive for the following logic:
    //
    if(a < 0)
        return float_distance(static_cast<T>(-b), static_cast<T>(-a));

//    ASSERT(a >= 0 && b >= a); // don't assert here -- this code is very efficiency sensitive

    int expon;
    //
    // Note that if a is a denorm then the usual formula fails
    // because we actually have fewer than digits<T>()
    // significant bits in the representation:
    //
    frExp((fpclassify(a) == FP_SUBNORMAL) ? std::numeric_limits<T>::min() : a, &expon);
    T upper = ldExp(T(1), expon);
    T result = T(0);
    expon = digits<T>() - expon;
    //
    // If b is greater than upper, then we *must* split the calculation
    // as the size of the ULP changes with each order of magnitude change:
    //
    if(b > upper) {
        result = float_distance(upper, b);
    }
    //
    // Use compensated double-double addition to avoid rounding
    // errors in the subtraction:
    //
    T mb, x, y, z;
    if((fpclassify(a) == FP_SUBNORMAL) || (b - a < std::numeric_limits<T>::min())) {
        //
        // Special case - either one end of the range is a denormal, or else the difference is.
        // The regular code will fail if we're using the SSE2 registers on Intel and either
        // the FTZ or DAZ flags are set.
        //
        T a2 = ldExp(a, digits<T>());
        T b2 = ldExp(b, digits<T>());
        mb = -std::min(ldExp(upper, digits<T>()), b2);
        x = a2 + mb;
        z = x - a2;
        y = (a2 - (x - z)) + (mb - z);

        expon -= digits<T>();
    } else {
        mb = -std::min(upper, b);
        x = a + mb;
        z = x - a;
        y = (a - (x - z)) + (mb - z);
    }
    if(x < 0)
    {
        x = -x;
        y = -y;
    }
    result += ldExp(x, expon) + ldExp(y, expon);

    //
    // Result must be an integer:
    //

    // ASSERT(result == floor(result)); // don't assert here -- this code is very efficiency sensitive

    return result;
}

}  // namespace detail

namespace fast {

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

}  // namespace fast

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
        return (1 + fast::float_distance(detail::abs(b), std::numeric_limits<T>::min())) <= maxUlpsDiff;
    }
    if (b == 0) {
        return (1 + fast::float_distance(detail::abs(a), std::numeric_limits<T>::min())) <= maxUlpsDiff;
    }

    if (signbit(a) == signbit(b)) return fast::float_distance(a, b) <= maxUlpsDiff;

    // If signs are different, add ULP distances from minimum normal number on both sides of 0
    return (2 + fast::float_distance(a > 0 ? a : b, std::numeric_limits<T>::min())
              + fast::float_distance(a < 0 ? a : b, -std::numeric_limits<T>::min())) <= maxUlpsDiff;
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
