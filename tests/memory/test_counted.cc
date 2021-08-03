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

#include "eckit/eckit.h"

#include "eckit/log/Log.h"
#include "eckit/memory/Counted.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

struct FooLock : public OwnedLock {
    typedef SharedPtr<FooLock> ptype;

    FooLock(int in) :
        i(in) {}
    int i;
};

struct FooNoLock : public OwnedNoLock {
    typedef SharedPtr<FooNoLock> ptype;

    FooNoLock(int in) :
        i(in) {}
    int i;
};

template <typename F>
void TestDefault() {

    typename F::ptype p;
    EXPECT(!p);
    EXPECT(p.get() == nullptr);

    p.reset(new F(10));

    EXPECT(p);
    EXPECT(p.unique());

    p.release();

    EXPECT(!p);
    EXPECT(p.owners() == 0);
}

template <typename F>
void TestCopy() {

    typename F::ptype p1;
    typename F::ptype p2;
    typename F::ptype p3;

    EXPECT(!p1);
    EXPECT(p1.owners() == 0);

    p1.reset(new F(10));

    EXPECT(p1->i == 10);

    p1->i = 20;

    EXPECT(p1 != p2);
    EXPECT(p1 != p3);
    EXPECT(p2 == p3);

    EXPECT(p1);
    EXPECT(!p2);
    EXPECT(!p3);

    EXPECT(p1.owners() == 1);
    EXPECT(p2.owners() == 0);
    EXPECT(p3.owners() == 0);

    p2.reset(p1);

    EXPECT(p1->i == 20);
    EXPECT(p2->i == 20);

    p1->i = 30;

    EXPECT(p1->i == 30);
    EXPECT(p2->i == 30);

    EXPECT(p1 == p2);
    EXPECT(p1 != p3);
    EXPECT(p2 != p3);

    EXPECT(p1);
    EXPECT(p2);
    EXPECT(!p3);

    EXPECT(p1.owners() == 2);
    EXPECT(p2.owners() == 2);
    EXPECT(p3.owners() == 0);

    p3 = p1;

    p1->i = 40;

    EXPECT(p1->i == 40);
    EXPECT(p2->i == 40);
    EXPECT(p3->i == 40);

    EXPECT(p1 == p2);
    EXPECT(p1 == p3);
    EXPECT(p2 == p3);

    EXPECT(p1);
    EXPECT(p2);
    EXPECT(p3);

    EXPECT(p1.owners() == 3);
    EXPECT(p2.owners() == 3);
    EXPECT(p3.owners() == 3);

    p1.release();

    EXPECT(!p1);
    EXPECT(p2);
    EXPECT(p3);

    EXPECT(p1.owners() == 0);
    EXPECT(p2.owners() == 2);
    EXPECT(p3.owners() == 2);

    p2.release();

    EXPECT(!p1);
    EXPECT(!p2);
    EXPECT(p3);

    EXPECT(p1.owners() == 0);
    EXPECT(p2.owners() == 0);
    EXPECT(p3.owners() == 1);

    p3.release();

    EXPECT(!p1);
    EXPECT(!p2);
    EXPECT(!p3);

    EXPECT(p1.owners() == 0);
    EXPECT(p2.owners() == 0);
    EXPECT(p3.owners() == 0);
}

template <typename F>
void TestRelease() {
    typename F::ptype p;

    EXPECT(!p);

    p.release();

    EXPECT(!p);

    p.reset(new F(10));

    EXPECT(p);

    p.release();

    EXPECT(!p);
}

template <typename F>
void TestSwap() {
    typename F::ptype p1(new F(10));
    typename F::ptype p2(new F(5));

    EXPECT(p1);
    EXPECT(p2);

    EXPECT(p1.unique());
    EXPECT(p2.unique());


    EXPECT(p1->i == 10);
    EXPECT(p2->i == 5);

    p1.swap(p2);

    EXPECT(p1->i == 5);
    EXPECT(p2->i == 10);

    EXPECT(p1.unique());
    EXPECT(p2.unique());

    EXPECT(p1);
    EXPECT(p2);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_default") {
    SECTION("Type FooLock") { TestDefault<FooLock>(); }
    SECTION("Type FooNoLock") { TestDefault<FooNoLock>(); }
}

CASE("test_copy") {
    SECTION("Type FooLock") { TestCopy<FooLock>(); }
    SECTION("Type FooNoLock") { TestCopy<FooNoLock>(); }
}

CASE("test_release") {
    SECTION("Type FooLock") { TestRelease<FooLock>(); }
    SECTION("Type FooNoLock") { TestRelease<FooNoLock>(); }
}

CASE("test_swap") {
    SECTION("Type FooLock") { TestSwap<FooLock>(); }
    SECTION("Type FooNoLock") { TestSwap<FooNoLock>(); }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
