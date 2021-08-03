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

#include <set>

#include "eckit/log/Log.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

class Incomplete;

Incomplete* get_ptr(eckit::SharedPtr<Incomplete>& incomplete) {
    return incomplete.get();
}

template <class T>
void ck(const T* v1, T v2) {
    EXPECT(*v1 == v2);
}

namespace {
int UDT_use_count;  // independent of pointer maintained counts
}

//----------------------------------------------------------------------------------------------------------------------
//  user defined type

class UDT : public OwnedNoLock {
public:
    typedef SharedPtr<UDT> ptype;

    UDT(long in) :
        value_(in) { ++UDT_use_count; }
    ~UDT() {
        --UDT_use_count;
        //       std::cout << "   UDT with value " << value_ << " being destroyed\n";
    }

    long value() const { return value_; }
    void value(long v) {
        value_ = v;
        ;
    }

private:
    long value_;
};

//----------------------------------------------------------------------------------------------------------------------
//  tests on incomplete types

//  Certain smart pointer operations are specified to work on incomplete types,
//  and some uses depend upon this feature.  These tests verify compilation
//  only - the functions aren't actually invoked.

class Incomplete;

Incomplete* check_incomplete(eckit::SharedPtr<Incomplete>& incomplete, eckit::SharedPtr<Incomplete>& i2) {
    incomplete.swap(i2);
    // std::cout << incomplete.use_count() << ' ' << incomplete.unique() << '\n';
    return incomplete.get();
}

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
eckit::SharedPtr<T> add_another_shareptr(T* p) {
    return eckit::SharedPtr<T>(p);
}

//----------------------------------------------------------------------------------------------------------------------

// TODO issues:
// o/ test shared ptr, in STL containers
// o/ <Not applicable> weak shared ptr
// o/ <Not applicable> enable_shared_from_this/ not required
// o/ <Not applicable> custom deletor
// o/ <Not applicable> ? static pointer cast
//
// eckit::SharedPtr has null() member function, this does not exist on the standard/boost

//----------------------------------------------------------------------------------------------------------------------

CASE("test_intrusive_shared_ptr") {

    //   std::cout << "test SharedPtr with a user defined type\n";
    {
        UDT_use_count = 0;
        UDT* up       = new UDT(0);
        EXPECT(up->value() == 0);

        eckit::SharedPtr<UDT> sup1(up);
        EXPECT(up == sup1.get());
        EXPECT(sup1.use_count() == 1);
        EXPECT(sup1.unique());

        sup1->value(54321);
        EXPECT(sup1->value() == 54321);
        EXPECT(up->value() == 54321);

        eckit::SharedPtr<UDT> sup2;
        sup2 = sup1;
        EXPECT(sup2->value() == 54321);
        EXPECT(sup1.use_count() == 2);
        EXPECT(sup2.use_count() == 2);
        EXPECT(!sup2.unique());
        EXPECT(!sup1.unique());

        //      cout << "eckit::SharedPtr check self assignment\n";
        sup2 = sup2;
        EXPECT(sup2->value() == 54321);
        EXPECT(sup1.use_count() == 2);
        EXPECT(sup2.use_count() == 2);

        // check return creates from function

        eckit::SharedPtr<UDT> sup3 = add_another_shareptr(up);
        EXPECT(sup3->value() == 54321);
        EXPECT(sup1.use_count() == 3);
        EXPECT(sup2.use_count() == 3);
        EXPECT(sup3.use_count() == 3);
    }

    EXPECT(UDT_use_count == 0);

    {
        //      std::cout << "test SharedPtr swap\n";
        eckit::SharedPtr<UDT> sup1(new UDT(0));
        EXPECT(sup1.get() != 0);
        EXPECT(sup1.use_count() == 1);
        EXPECT(sup1.unique());

        eckit::SharedPtr<UDT> sup2;
        EXPECT(sup2.use_count() == 0);
        EXPECT(sup2.get() == 0);

        sup1.swap(sup2);

        EXPECT(sup2.get() != 0);
        EXPECT(sup2.use_count() == 1);
        EXPECT(sup2.unique());

        EXPECT(sup1.use_count() == 0);
        EXPECT(sup1.get() == 0);
    }

    EXPECT(UDT_use_count == 0);


    //   std::cout << "test SharedPtr with a user defined type in std::vector\n";
    {
        std::vector<eckit::SharedPtr<UDT> > vec;
        vec.push_back(eckit::SharedPtr<UDT>(new UDT(0)));
        vec.push_back(eckit::SharedPtr<UDT>(new UDT(1)));
        vec.push_back(eckit::SharedPtr<UDT>(new UDT(2)));
    }
    EXPECT(UDT_use_count == 0);

    //   std::cout << "test SharedPtr with a user defined type in std::set\n";
    {
        std::set<eckit::SharedPtr<UDT> > vec;
        vec.insert(eckit::SharedPtr<UDT>(new UDT(0)));
        vec.insert(eckit::SharedPtr<UDT>(new UDT(1)));
        vec.insert(eckit::SharedPtr<UDT>(new UDT(3)));
    }
    EXPECT(UDT_use_count == 0);

    //   std::cout << "test SharedPtr with a user defined type in std::map\n";
    {
        std::map<std::string, eckit::SharedPtr<UDT> > map;
        map.insert(std::make_pair(std::string("first"), eckit::SharedPtr<UDT>(new UDT(0))));
        map.insert(std::make_pair(std::string("secon"), eckit::SharedPtr<UDT>(new UDT(1))));
        map.insert(std::make_pair(std::string("third"), eckit::SharedPtr<UDT>(new UDT(2))));
    }
    EXPECT(UDT_use_count == 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
