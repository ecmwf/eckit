/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define ECKIT_NO_DEPRECATION_WARNINGS

#include <cmath>
#include <iostream>


#include "eckit/log/Log.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

/// These tests are similar to the test for boost scoped_ptr and shared ptrs
/// This allows as in the future to drop out, our own home grown managed
/// ptr's in favour of the standards.

//----------------------------------------------------------------------------------------------------------------------

template <class T>
void ck(const T* v1, T v2) {
    EXPECT(*v1 == v2);
}

namespace {
static int UDT_use_count = 0;  // independent of pointer maintained counts
}

//----------------------------------------------------------------------------------------------------------------------
//  user defined type

class UDT {
    long value_;

public:
    explicit UDT(long value = 0) :
        value_(value) { ++UDT_use_count; }
    ~UDT() {
        --UDT_use_count;
        //      std::cout << "   UDT with value " << value_ << " being destroyed\n";
    }
    long value() const { return value_; }
    void value(long v) { value_ = v; }
};

//----------------------------------------------------------------------------------------------------------------------
//  tests on incomplete types

//  Certain smart pointer operations are specified to work on incomplete types,
//  and some uses depend upon this feature.  These tests verify compilation
//  only - the functions aren't actually invoked.

class Incomplete;

Incomplete* check_incomplete(ScopedPtr<Incomplete>& incomplete) {
    return incomplete.get();
}

//----------------------------------------------------------------------------------------------------------------------

// TODO issues:
// o/ test shared ptr, in STL containers
// o/ <Not applicable> weak shared ptr
// o/ <Not applicable> enable_shared_from_this/ not required
// o/ <Not applicable> custom deletor
// o/ <Not applicable> ? static pointer cast

CASE("test_scoped_ptr_empty_constructor") {
    ScopedPtr<long> sp;

    EXPECT(sp.get() == ScopedPtr<long>::pointer_type(0));

    EXPECT(sp.release() == ScopedPtr<long>::pointer_type(0));

    EXPECT(!sp);

    EXPECT_NO_THROW(sp.reset());
}

CASE("test_scoped_ptr") {
    //   std::cout << "test ScopedPtr with a built-in type\n";
    {
        long* lp = new long;
        ScopedPtr<long> sp(lp);
        EXPECT(sp);
        EXPECT(sp.get() == lp);
        EXPECT(lp == sp.get());
        EXPECT(&*sp == lp);

        *sp = 1234568901L;
        EXPECT(*sp == 1234568901L);
        EXPECT(*lp == 1234568901L);
        ck(static_cast<long*>(sp.get()), 1234568901L);
        ck(lp, *sp);

        sp.reset();

        EXPECT(sp.get() == ScopedPtr<long>::pointer_type(0));
    }

    //   std::cout << "test ScopedPtr with a user defined type\n";
    {
        ScopedPtr<UDT> udt_sp(new UDT(999888777));
        EXPECT(udt_sp->value() == 999888777);
        udt_sp.reset();
        udt_sp.reset(new UDT(111222333));
        EXPECT(udt_sp->value() == 111222333);
        udt_sp.reset(new UDT(333222111));
        EXPECT(udt_sp->value() == 333222111);

        udt_sp.reset();
        EXPECT(udt_sp.get() == ScopedPtr<UDT>::pointer_type(0));
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
