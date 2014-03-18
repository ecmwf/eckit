#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

#include "FloatCompare.h"

using namespace std;

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------

// See:
// * http://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format
// * http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition
//   for the potential portability problems with the union and bit-fields below.

/// @TODO assert sizeof(float.i) == sizeof(float.f)
/// @TODO check in BIGENDIAN archs...

union Float_t
{
    Float_t(float num = 0.0f) : f(num) {}
    // Portable extraction of components.
    bool Negative() const { return (i >> 31) != 0; }
    int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
    int32_t RawExponent() const { return (i >> 23) & 0xFF; }

    int32_t i;
    float f;
};

union Double_t
{
    Double_t( double num = 0.0 ) : f(num) {}
    // Portable extraction of components.
    bool Negative() const { return (i >> 31) != 0; }
    int64_t RawMantissa() const { return i & ((1 << 23) - 1); }
    int64_t RawExponent() const { return (i >> 23) & 0xFF; }

    int64_t i;
    double  f;
};

bool AlmostEqualUlps(float A, float B, int maxUlpsDiff)
{
    Float_t uA(A);
    Float_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
    {
        // Check for equality to make sure +0==-0
        if (A == B)
            return true;
        return false;
    }

    // Find the difference in ULPs.
    int ulpsDiff = std::abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool AlmostEqualUlps(double A, double B, int maxUlpsDiff)
{
    Double_t uA(A);
    Double_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
    {
        // Check for equality to make sure +0==-0
        if (A == B)
            return true;
        return false;
    }

    // Find the difference in ULPs.
    int ulpsDiff = std::abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool AlmostEqualUlpsAndAbs(float A, float B, float maxDiff, int maxUlpsDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float absDiff = fabs(A - B);
    if (absDiff <= maxDiff)
        return true;

    Float_t uA(A);
    Float_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
        return false;

    // Find the difference in ULPs.
    int ulpsDiff = std::abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

bool AlmostEqualRelativeAndAbs(float A, float B, float maxDiff, float maxRelDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float diff = fabs(A - B);
    if (diff <= maxDiff)
        return true;

    A = fabs(A);
    B = fabs(B);
    float largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}

bool AlmostEqualUlpsAndAbs(double A, double B, double maxDiff, int maxUlpsDiff)
{
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    double absDiff = std::abs(A - B);
    if (absDiff <= maxDiff)
        return true;

    Double_t uA(A);
    Double_t uB(B);

    // Different signs means they do not match.
    if (uA.Negative() != uB.Negative())
        return false;

    // Find the difference in ULPs.
    int ulpsDiff = std::abs(uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff)
        return true;

    return false;
}

//-----------------------------------------------------------------------------

bool FloatCompare::is_equal(double a, double b)
{
    //    return AlmostEqualUlps(a,b,10);
    return AlmostEqualUlpsAndAbs(a,b,std::numeric_limits<double>::epsilon(),10);
}

//-----------------------------------------------------------------------------

} // namespace eckit
