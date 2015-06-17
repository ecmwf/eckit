/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Jun 2015

#ifndef eckit_FloatCompare_h
#define eckit_FloatCompare_h

#include <limits>


namespace eckit {

//---------------------------------------------------------------------------------------------------------

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


/// This function only has specializations for double and float
/// So it restricts the use of FloatCompare
template< typename T >
bool isApproxEqualUlps( T a, T b, T epsilon = std::numeric_limits<T>::epsilon(), int maxUlpsDiff = 10 );


/// Comparison class for Floating Point numbers
/// Type T should be either float or double
template< typename T >
class FloatCompare {
public:

	/// Compare values equality
	static bool isEqual(const T& a,  const T& b)  { return (a == b); }

  /// Compare values inequality: "is greater or equal to"
  static bool isGreaterEqual(const T& a,  const T& b) { return (a >= b); }

  /// Compare values inequality: "is greater or approximately equal to"
  static bool isGreaterApproxEqual(const T& a,  const T& b) { return (a >= b) || isApproxEqual(a,b); }

	/// Compare values equality, approximately
	static bool isApproxEqual( T a,
							T b,
							T epsilon = std::numeric_limits<float>::epsilon(),
							int maxUlpsDiff = 10 ) { return isApproxEqualUlps(a,b,epsilon,maxUlpsDiff); }

};


/// Helper class that memorizes the comparison setttings
template < typename T >
class FloatApproxCompare {

	T eps_;
	int maxUlps_;

public:

	FloatApproxCompare( T eps = std::numeric_limits<T>::epsilon(), int maxUlps = 10 ) :
		eps_(eps),
		maxUlps_(maxUlps)
	{}

	bool operator() ( const T& a,  const T& b) { return FloatCompare<T>::isApproxEqual(a,b,eps_,maxUlps_); }

};


//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

