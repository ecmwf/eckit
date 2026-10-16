// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <exception>
#include <stdexcept>
#include <thread>

#include "eckit/exception/Exceptions.h"

#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

std::string evaluate_message(bool answer_is_correct) {
    if (answer_is_correct) {
        // this should never execute, and therefore the code below never reached
        throw NotImplemented(Here());
// Suppress warning "statement is unreachable"
#if defined(__NVCOMPILER)
#pragma diag_suppress 111
#endif
        return "NON REACHABLE CODE";
    }

    std::cout << "THIS SHOULD PRINT" << std::endl;
    return "ASSERT FAILS";
}

CASE("Test AssertionFailed") {
    EXPECT_NO_THROW(ASSERT(true));
    EXPECT_THROWS_AS(ASSERT(false), AssertionFailed);
}

CASE("Test FunctionalityNotSupported") {
    struct A {
        virtual void method() { throw FunctionalityNotSupported("not supported"); }
    } a;

    struct B final : A {
        void method() override {}
    } b;

    // "not supported" is also "not implemented"
    EXPECT_THROWS_AS(a.method(), FunctionalityNotSupported);
    EXPECT_THROWS_AS(a.method(), NotImplemented);

    EXPECT_NO_THROW(b.method());
}

CASE("Test ASSERT_MSG does not evaluate message if assertion not triggered") {

    bool notfail = true;
    EXPECT_NO_THROW(ASSERT_MSG(notfail, evaluate_message(notfail)));

    notfail = false;
    EXPECT_THROWS_AS(ASSERT_MSG(notfail, evaluate_message(notfail)), AssertionFailed);
}

namespace {

/// Records, when destroyed, what Exception::throwing() says.
struct RecordThrowing {
    bool& throwing;
    ~RecordThrowing() { throwing = Exception::throwing(); }
};

template <typename Thrower>
bool throwingWhileUnwinding(Thrower&& thrower) {
    bool throwing = false;
    try {
        RecordThrowing record{throwing};
        thrower();
    }
    catch (...) {
    }
    return throwing;
}

}  // namespace

CASE("Exception::throwing: is false when nothing propagates") {
    bool throwing = true;
    { RecordThrowing record{throwing}; }
    EXPECT(!throwing);
}

CASE("Exception::throwing: is true while an exception of any type propagates") {
    EXPECT(throwingWhileUnwinding([] { throw std::runtime_error{"standard"}; }));
    EXPECT(throwingWhileUnwinding([] { throw SeriousBug{"eckit"}; }));
    EXPECT(throwingWhileUnwinding([] { throw 42; }));
}

CASE("Exception::throwing: is false inside a catch block") {
    bool throwing = true;
    try {
        throw SeriousBug{"caught"};
    }
    catch (const Exception&) {
        RecordThrowing record{throwing};
    }
    EXPECT(!throwing);
}

CASE("Exception::throwing: is false on a thread after an exception it held was released elsewhere") {
    std::exception_ptr held;
    std::atomic<int> stage{0};
    bool throwing = true;

    std::thread worker{[&] {
        try {
            throw SeriousBug{"transported"};
        }
        catch (...) {
            held = std::current_exception();
        }
        stage.store(1, std::memory_order_release);
        while (stage.load(std::memory_order_acquire) != 2) {
            std::this_thread::yield();
        }
        RecordThrowing record{throwing};
    }};

    while (stage.load(std::memory_order_acquire) != 1) {
        std::this_thread::yield();
    }
    held = nullptr;  // the last reference is released on another thread
    stage.store(2, std::memory_order_release);
    worker.join();

    EXPECT(!throwing);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
