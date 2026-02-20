/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/memory/Counted.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

struct TestCounted : public Counted {
    TestCounted(int val, bool& destroyed) : i(val), destroyed_(destroyed) { destroyed_ = false; }

    ~TestCounted() override { destroyed_ = true; }

    int i;
    bool& destroyed_;
};

//----------------------------------------------------------------------------------------------------------------------

CASE("test_count_starts_at_zero") {
    bool destroyed = false;
    auto* obj      = new TestCounted(42, destroyed);

    EXPECT(obj->count() == 0);

    // Must attach before detach to trigger cleanup
    obj->attach();
    obj->detach();
    EXPECT(destroyed);
}

CASE("test_attach_detach") {
    bool destroyed = false;
    auto* obj      = new TestCounted(10, destroyed);

    obj->attach();
    EXPECT(obj->count() == 1);
    EXPECT(!destroyed);

    obj->detach();  // count reaches 0, triggers delete
    EXPECT(destroyed);
}

CASE("test_multiple_attach") {
    bool destroyed = false;
    auto* obj      = new TestCounted(20, destroyed);

    obj->attach();
    EXPECT(obj->count() == 1);

    obj->attach();
    EXPECT(obj->count() == 2);

    obj->attach();
    EXPECT(obj->count() == 3);

    obj->detach();
    EXPECT(obj->count() == 2);
    EXPECT(!destroyed);

    obj->detach();
    EXPECT(obj->count() == 1);
    EXPECT(!destroyed);

    obj->detach();  // count reaches 0, triggers delete
    EXPECT(destroyed);
}

CASE("test_data_access") {
    bool destroyed = false;
    auto* obj      = new TestCounted(99, destroyed);

    obj->attach();
    EXPECT(obj->i == 99);

    obj->i = 42;
    EXPECT(obj->i == 42);

    obj->detach();
    EXPECT(destroyed);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
