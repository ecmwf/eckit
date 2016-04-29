#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>

#include "eckit/types/FloatCompare.h"

using namespace std;

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// See:
// * http://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format
// * http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition
//   for the potential portability problems with the union and bit-fields below.
//
// Format of IEEE floating-point:
//
//   The most-significant bit being the leftmost, an IEEE
//   floating-point looks like
//
//     sign_bit exponent_bits fraction_bits
//
//   Here, sign_bit is a single bit that designates the sign of the
//   number.
//
//   For float, there are 8 exponent bits and 23 fraction bits.
//
//   For double, there are 11 exponent bits and 52 fraction bits.
//
//   More details can be found at
//   http://en.wikipedia.org/wiki/IEEE_floating-point_standard.


/// @TODO check in BIGENDIAN archs...

// Used for accessing the integer representation of floating-point numbers
// (aliasing through unions works on most platforms).
union FloatType
{
    FloatType(float number = 0.0f) : f_(number) { assert( sizeof(i_) == sizeof(f_) ); }

    bool    negative()    const { return (i_ >> 31) != 0; }
    int32_t rawMantissa() const { return i_ & ((1 << 23) - 1); }
    int32_t rawExponent() const { return (i_ >> 23) & 0xFF; }

    int32_t i_;
    float   f_;
};

// Used for accessing the integer representation of floating-point numbers
// (aliasing through unions works on most platforms).
union Double
{
    Double(double number = 0.0) : f_(number) { assert( sizeof(i_) == sizeof(f_) ); }

    bool    negative()    const { return (i_ >> 63) != 0; }
    int64_t rawMantissa() const { return i_ & ((1LL << 52) - 1); }
    int64_t rawExponent() const { return (i_ >> 52) & 0x7FF; }

    int64_t i_;
    double  f_;
};

//// Allow a common algorithm to compare floats and doubles, by using templates
template < class T > struct FPCompare;

template <> struct FPCompare<float>  {
   typedef FloatType  FP_t;
   typedef int32_t int_type;
   static int_type diff(int_type a, int_type b) { int_type x = a - b; return ((int_type)(((x) >= 0) ? (x) : -(x))); }
};

template <> struct FPCompare<double> {
   typedef Double FP_t;
   typedef int64_t int_type;
   static int_type diff(int_type a, int_type b) { int_type x = a - b; return ((int_type)(((x) >= 0) ? (x) : -(x))); }
};

template < typename T >
bool AlmostEqualUlps(T A, T B, int maxUlpsDiff)
{
    // Ensure that either A or B are not close to zero.
    // ULP based equality breaks down, near zero
    assert( (fabs(A) > 5 * std::numeric_limits<T>::epsilon()) || (fabs(B) > 5 * std::numeric_limits<T>::epsilon()) );

    // Initialise unions with floats
    typename FPCompare<T>::FP_t uA(A);
    typename FPCompare<T>::FP_t uB(B);

    // Different signs means they do not match.
    // ULP comparison does not make sense when signs are different
    if (uA.negative() != uB.negative())
    {
        // Check for equality to make sure +0==-0
        if (A == B)
            return true;
        return false;
    }

    // Find the difference in ULPs.
    typename FPCompare<T>::int_type ulpsDiff = FPCompare<T>::diff(uA.i, uB.i);
    if ( ulpsDiff  <= maxUlpsDiff)
       return true;

    return false;
}

/// Compare 2 floats with an absolute epsilon check (values near zero), then based on ULPs
///
/// A, B       : two floating-point numbers (single/double precision) to compare.
/// maxDiff    : epsilon for floating-point absolute epsilon check (should be some
///              small multiple of std::numeric_limits<T>::epsilon().
/// maxUlpsDiff: unit in the last place or unit of least precision.
///              maxUlpsDiff can also be interpreted in terms of how many representable floats we are
///              willing to accept between A and B. This function will allow maxUlpsDiff-1 floats
///              between A and B.
///
/// If two numbers are identical except for a one-bit difference in the last digit of their mantissa
/// then this function will calculate ulpsDiff as one.
/// If one number is the maximum number for a particular exponent, perhaps 1.99999988,
/// and the other number is the smallest number for the next exponent, say 2.0,
/// then this function will again calculate ulpsDiff as one.
/// In both cases the two numbers are the closest floats there are.
///
template < typename T >
bool AlmostEqualUlpsAndAbs(T A, T B, T maxDiff, int maxUlpsDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    T absDiff = fabs(A - B);
    if (absDiff <= maxDiff)
        return true;

    // Initialise unions with floats
    typename FPCompare<T>::FP_t uA(A);
    typename FPCompare<T>::FP_t uB(B);

    // Different signs means they do not match.
    // ULP comparison does not make sense when sign are different
    if (uA.negative() != uB.negative())
        return false;

    // Find the difference in ULPs, taking note to use correct int_type for the difference, to avoid overflow
    typename FPCompare<T>::int_type ulpsDiff = FPCompare<T>::diff(uA.i_, uB.i_);
    if ( ulpsDiff <= maxUlpsDiff)
       return true;

    return false;
}

template < typename T >
bool AlmostEqualRelativeAndAbs(T A, T B, T maxDiff, T maxRelDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    T diff = fabs(A - B);
    if (diff <= maxDiff)
        return true;

    A = fabs(A);
    B = fabs(B);
    T largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}

template<>
bool isApproxEqualUlps(float a, float b, float epsilon, int maxUlpsDiff)
{
    return AlmostEqualUlpsAndAbs(a,b,epsilon,maxUlpsDiff);
}

template<>
bool isApproxEqualUlps(double a, double b, double epsilon, int maxUlpsDiff)
{
    return AlmostEqualUlpsAndAbs(a,b,epsilon,maxUlpsDiff);
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
