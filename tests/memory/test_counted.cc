// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
