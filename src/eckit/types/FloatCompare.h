#ifndef eckit_FloatCompare_h
#define eckit_FloatCompare_h

#include <limits>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FloatCompare {
public:
   /// Compare 2 floats with an absolute epsilon check (values near zero), then based on ULPs
   ///
   /// epsilon    : epsilon for floating-point absolute epsilon check (should be some
   ///              small multiple of std::numeric_limits<T>::epsilon().
   ///              Used to check values near zero
   /// maxUlpsDiff: maximum spacing between the floating-point numbers A and B
   ///              unit in the last place or unit of least precision. defaults to 10.
   ///              maxUlpsDiff can also be interpreted in terms of how many representable floats we are
   ///              willing to accept between A and B. This function will allow maxUlpsDiff-1 floats
   ///              between A and B.
   ///
   /// Assumptions:
   ///   o These functions *ignores* NAN, assumes we have enabled illegal operation exceptions,
   ///     i.e so that you find out when you generate them.
   ///   o Ignores infinities and overflow exceptions.
   ///
   /// Grib
   ///   In grib files latitudes and longitudes, are angles stored as doubles
   ///   However we need to carefully choose the right epsilon.
   ///     Grib1: epsilon  0.001
   ///     Grib2: precision is user defined, but generally we can rely on using epsilon = 1e-6  ?
   ///
   static bool is_equal( float a,
                            float b,
                            float epsilon = std::numeric_limits<float>::epsilon(),
                            int maxUlpsDiff = 10 );
   static bool is_equal( double a,
                            double b,
                            double epsilon = std::numeric_limits<double>::epsilon(),
                            int maxUlpsDiff = 10 );
};

//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

