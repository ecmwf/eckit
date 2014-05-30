/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <iostream>

#define BOOST_TEST_MODULE test_scoped_ptr

#include <boost/test/included/unit_test.hpp>

#include "eckit/memory/Owned.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

using namespace std;
using namespace eckit;

/// These tests are similar to the test for boost scoped_ptr and shared ptrs
/// This allows as in the future to drop out, our own home grown managed
/// ptr's in favour of the standards.

//-----------------------------------------------------------------------------

namespace eckit_test {

template<class T>
void ck( const T* v1, T v2 ) { BOOST_CHECK( *v1 == v2 ); }

namespace {
   static int UDT_use_count = 0;  // independent of pointer maintained counts
}

//-----------------------------------------------------------------------------
//  user defined type

class UDT {
   long value_;
public:
   explicit UDT( long value=0 ) : value_(value) { ++UDT_use_count; }
   ~UDT() {
      --UDT_use_count;
      std::cout << "   UDT with value " << value_ << " being destroyed\n";
   }
   long value() const { return value_; }
   void value( long v ) { value_ = v; }
};

//-----------------------------------------------------------------------------
//  tests on incomplete types

//  Certain smart pointer operations are specified to work on incomplete types,
//  and some uses depend upon this feature.  These tests verify compilation
//  only - the functions aren't actually invoked.

class Incomplete;

Incomplete * check_incomplete( eckit::ScopedPtr<Incomplete>& incomplete )
{
   return incomplete.get();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

// TODO issues:
// o/ test shared ptr, in STL containers
// o/ <Not applicable> weak shared ptr
// o/ <Not applicable> enable_shared_from_this/ not required
// o/ <Not applicable> custom deletor
// o/ <Not applicable> ? static pointer cast

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( TestScoped )

BOOST_AUTO_TEST_CASE( test_scoped_ptr )
{
   std::cout << "test ScopedPtr with a built-in type\n";
   {
      long * lp = new long;
      eckit::ScopedPtr<long> sp ( lp );
      BOOST_CHECK( sp );
      BOOST_CHECK( sp.get() == lp );
      BOOST_CHECK( lp == sp.get() );
      BOOST_CHECK( &*sp == lp );

      *sp = 1234568901L;
      BOOST_CHECK( *sp == 1234568901L );
      BOOST_CHECK( *lp == 1234568901L );
      ck( static_cast<long*>(sp.get()), 1234568901L );
      ck( lp, *sp );

      sp.reset();

      BOOST_CHECK( sp.get() == 0 );
   }

   std::cout << "test ScopedPtr with a user defined type\n";
   {
      eckit::ScopedPtr<UDT> udt_sp ( new UDT( 999888777 ) );
      BOOST_CHECK( udt_sp->value() == 999888777 );
      udt_sp.reset();
      udt_sp.reset( new UDT( 111222333 ) );
      BOOST_CHECK( udt_sp->value() == 111222333 );
      udt_sp.reset( new UDT( 333222111 ) );
      BOOST_CHECK( udt_sp->value() == 333222111 );

      udt_sp.reset();
      BOOST_CHECK( udt_sp.get() == 0 );
   }
}

BOOST_AUTO_TEST_SUITE_END()
