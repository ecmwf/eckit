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

#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
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

class Incomplete;

Incomplete * get_ptr(  eckit::SharedPtr<Incomplete>& incomplete )
{
  return incomplete.get();
}

template<class T>
void ck( const T* v1, T v2 ) { ASSERT( *v1 == v2 ); }

namespace {
   int UDT_use_count;  // independent of pointer maintained counts
}

/// ===========================================================================
//  user defined type  -------------------------------------------------------//

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
};  // UDT


class UDTIntrusiveNoLock : public OwnedNoLock {
public:
    typedef SharedPtr<UDTIntrusiveNoLock> ptype;

    UDTIntrusiveNoLock( long in ) : value_(in) {  ++UDT_use_count; }
    ~UDTIntrusiveNoLock() {
       --UDT_use_count;
       std::cout << "   UDTIntrusiveNoLock with value " << value_ << " being destroyed\n";
    }

    long value() const { return value_; }
    void value( long v ) { value_ = v;; }
private:
    long value_;
};

//  tests on incomplete types  -----------------------------------------------//

//  Certain smart pointer operations are specified to work on incomplete types,
//  and some uses depend upon this feature.  These tests verify compilation
//  only - the functions aren't actually invoked.

class Incomplete;

Incomplete * check_incomplete( eckit::ScopedPtr<Incomplete>& incomplete )
{
   return incomplete.get();
}

Incomplete * check_incomplete( eckit::SharedPtr<Incomplete>& incomplete,
                                 eckit::SharedPtr<Incomplete>& i2 )
{
   incomplete.swap(i2);
   //std::cout << incomplete.use_count() << ' ' << incomplete.unique() << '\n';
   return incomplete.get();
}

//-----------------------------------------------------------------------------

class TestShared : public Tool {
public:

   TestShared(int argc,char **argv): Tool(argc,argv) { UDT_use_count = 0;}

    ~TestShared() {}

    virtual void run();

private:
    void test_scoped_ptr();
    void test_intrusive_shared_ptr();
};

//-----------------------------------------------------------------------------

// TODO issues:
// o/ test shared ptr, in STL containers
// o/ <Not applicable> weak shared ptr
// o/ <Not applicable> enable_shared_from_this/ not required
// o/ <Not applicable> custom deletor
// o/ <Not applicable> ? static pointer cast
//
// eckit::SharedPtr has null() member function, this does not exist on the standard/boost

// ------------------------------------------------------------------------------

void TestShared::run()
{
   std::cout << "Start TestShared\n";

   ASSERT( UDT_use_count == 0 );  // reality check

   test_scoped_ptr();
   test_intrusive_shared_ptr();

   std::cout << "OK\n";
}


void TestShared::test_scoped_ptr()
{
   std::cout << "test ScopedPtr with a built-in type\n";
   {
      long * lp = new long;
      eckit::ScopedPtr<long> sp ( lp );
      ASSERT( sp );
      ASSERT( sp.get() == lp );
      ASSERT( lp == sp.get() );
      ASSERT( &*sp == lp );

      *sp = 1234568901L;
      ASSERT( *sp == 1234568901L );
      ASSERT( *lp == 1234568901L );
      ck( static_cast<long*>(sp.get()), 1234568901L );
      ck( lp, *sp );

      sp.reset();

      ASSERT( sp.get() == 0 );
   }

   std::cout << "test ScopedPtr with a user defined type\n";
   {
      eckit::ScopedPtr<UDT> udt_sp ( new UDT( 999888777 ) );
      ASSERT( udt_sp->value() == 999888777 );
      udt_sp.reset();
      udt_sp.reset( new UDT( 111222333 ) );
      ASSERT( udt_sp->value() == 111222333 );
      udt_sp.reset( new UDT( 333222111 ) );
      ASSERT( udt_sp->value() == 333222111 );

      udt_sp.reset();
      ASSERT( udt_sp.get() == 0 );
   }
}


void TestShared::test_intrusive_shared_ptr()
{
   std::cout << "test SharedPtr with a user defined type\n";
   {
      UDT_use_count = 0;
      UDTIntrusiveNoLock * up = new UDTIntrusiveNoLock(0);
      ASSERT( up->value() == 0 );

      eckit::SharedPtr<UDTIntrusiveNoLock> sup ( up );
      ASSERT( up == sup.get() );
      ASSERT( sup.use_count() == 1 );
      ASSERT( sup.unique());

      sup->value( 54321 ) ;
      ASSERT( sup->value() == 54321 );
      ASSERT( up->value() == 54321 );

      eckit::SharedPtr<UDTIntrusiveNoLock> sup2;
      sup2 = sup;
      ASSERT( sup2->value() == 54321 );
      ASSERT( sup.use_count() == 2 );
      ASSERT( sup2.use_count() == 2 );
      ASSERT( !sup2.unique());
      ASSERT( !sup.unique());

      // eckit::SharedPtr does not handle self assignment?
//      sup2 = sup2;
//      ASSERT( sup2->value() == 54321 );
//      ASSERT( sup.use_count() == 2 );
//      ASSERT( sup2.use_count() == 2 );
   }
   ASSERT(UDT_use_count == 0 );

   {
      std::cout << "test SharedPtr swap\n";
      eckit::SharedPtr<UDTIntrusiveNoLock> sup ( new UDTIntrusiveNoLock(0) );
      ASSERT(sup.get() != 0);
      ASSERT(sup.use_count() == 1 );
      ASSERT(sup.unique());

      eckit::SharedPtr<UDTIntrusiveNoLock> sup2;
      ASSERT(sup2.use_count() == 0 );
      ASSERT(sup2.get() == 0);

      sup.swap(sup2);

      ASSERT(sup2.get() != 0);
      ASSERT(sup2.use_count() == 1 );
      ASSERT(sup2.unique());

      ASSERT(sup.use_count() == 0 );
      ASSERT(sup.get() == 0);
   }


   std::cout << "test SharedPtr with a user defined type in std::vector\n";
   {
      std::vector< eckit::SharedPtr<UDTIntrusiveNoLock> > vec;
      vec.push_back(eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(0)));
      vec.push_back(eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(1)));
      vec.push_back(eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(2)));
   }
   ASSERT(UDT_use_count == 0 );
   std::cout << "test SharedPtr with a user defined type in std::set\n";
   {
      std::set< eckit::SharedPtr<UDTIntrusiveNoLock> > vec;
      vec.insert(eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(0)));
      vec.insert(eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(1)));
      vec.insert(eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(3)));
   }
   ASSERT(UDT_use_count == 0 );
   std::cout << "test SharedPtr with a user defined type in std::map\n";
   {
      std::map<std::string, eckit::SharedPtr<UDTIntrusiveNoLock> > map;
      map.insert(std::make_pair(std::string("first"),eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(0))));
      map.insert(std::make_pair(std::string("secon"),eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(1))));
      map.insert(std::make_pair(std::string("third"),eckit::SharedPtr<UDTIntrusiveNoLock>(new UDTIntrusiveNoLock(2))));
   }
   ASSERT(UDT_use_count == 0 );
}


//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    using namespace eckit_test;

    TestShared mytest(argc,argv);
    mytest.start();

    return 0;
}
