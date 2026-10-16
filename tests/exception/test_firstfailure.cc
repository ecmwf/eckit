// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/exception/FirstFailure.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

namespace {

void appendToString(void* data, const char* message) {
    static_cast<std::string*>(data)->append(message);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FirstFailure: nothing is thrown when every step succeeds") {
    int ran = 0;

    FirstFailure failure;
    failure.run([&] { ++ran; });
    failure.run([&] { ++ran; });

    EXPECT_NO_THROW(failure.rethrow());
    EXPECT_EQUAL(ran, 2);
}

CASE("FirstFailure: every step runs after a failure, and the first failure is the one rethrown") {
    std::vector<std::string> ran;

    FirstFailure failure;
    failure.run([&] {
        ran.push_back("first");
        throw std::runtime_error{"first failure"};
    });
    failure.run([&] {
        ran.push_back("second");
        throw std::logic_error{"second failure"};
    });
    failure.run([&] { ran.push_back("third"); });

    EXPECT_EQUAL(ran.size(), size_t{3});

    std::string message;
    try {
        failure.rethrow();
    }
    catch (const std::exception& error) {
        message = error.what();
    }
    EXPECT_EQUAL(message, std::string{"first failure"});
}

CASE("FirstFailure: the failure keeps its type and is rethrown once") {
    FirstFailure failure;
    failure.run([] { throw UserError("user", Here()); });

    EXPECT_THROWS_AS(failure.rethrow(), UserError);
    EXPECT_NO_THROW(failure.rethrow());
}

CASE("FirstFailure: failures of any type are kept") {
    FirstFailure failure;
    failure.run([] { throw 42; });

    int value = 0;
    try {
        failure.rethrow();
    }
    catch (int thrown) {
        value = thrown;
    }
    EXPECT_EQUAL(value, 42);
}

CASE("FirstFailure: a failure after a successful step is kept") {
    FirstFailure failure;
    failure.run([] {});
    failure.run([] { throw std::runtime_error{"late"}; });

    EXPECT_THROWS_AS(failure.rethrow(), std::runtime_error);
}

CASE("FirstFailure: a held failure does not make a later step look like an exception in flight") {
    FirstFailure failure;
    failure.run([] { throw SeriousBug{"held"}; });

    bool throwing = true;
    failure.run([&] { throwing = Exception::throwing(); });

    EXPECT(!throwing);
    EXPECT_THROWS_AS(failure.rethrow(), SeriousBug);
}

CASE("FirstFailure: the failures it drops are logged, and the one it keeps is not") {
    std::string logged;
    Log::error().setCallback(&appendToString, &logged);

    FirstFailure failure;
    failure.run([] { throw std::runtime_error{"kept failure"}; });
    failure.run([] { throw std::logic_error{"dropped failure"}; });
    failure.run([] { throw 42; });

    Log::error().reset();

    EXPECT(logged.find("dropped failure") != std::string::npos);
    EXPECT(logged.find("kept failure") == std::string::npos);
    EXPECT_THROWS_AS(failure.rethrow(), std::runtime_error);
}

CASE("FirstFailure: is neither copyable nor movable, as a copy would report the failure twice") {
    static_assert(!std::is_copy_constructible_v<FirstFailure>);
    static_assert(!std::is_copy_assignable_v<FirstFailure>);
    static_assert(!std::is_move_constructible_v<FirstFailure>);
    static_assert(!std::is_move_assignable_v<FirstFailure>);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
