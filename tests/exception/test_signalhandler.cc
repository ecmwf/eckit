// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <csignal>
#include <stdexcept>

#include "eckit/exception/Exceptions.h"
#include "eckit/os/SignalHandler.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

namespace {

/// Calls SignalHandler::checkInterrupt() when destroyed, and records whether it threw.
struct CheckInterruptOnDestruction {
    bool& threw;
    ~CheckInterruptOnDestruction() {
        try {
            SignalHandler::checkInterrupt();
        }
        catch (const Cancel&) {
            threw = true;
        }
    }
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

// checkInterrupt() relies on Exception::throwing(), which is based on std::uncaught_exceptions().

CASE("SignalHandler::checkInterrupt: throws Cancel inside a catch block") {
    SignalHandler handler;
    ::raise(SIGINT);

    bool cancelled = false;
    try {
        throw std::runtime_error{"handled"};
    }
    catch (const std::runtime_error&) {
        try {
            SignalHandler::checkInterrupt();
        }
        catch (const Cancel&) {
            cancelled = true;
        }
    }
    EXPECT(cancelled);
    EXPECT_NO_THROW(SignalHandler::checkInterrupt());
}

CASE("SignalHandler::checkInterrupt: does not throw while an exception propagates, and keeps the interrupt") {
    SignalHandler handler;
    ::raise(SIGINT);

    bool threw = false;
    try {
        CheckInterruptOnDestruction check{threw};
        throw std::runtime_error{"propagating"};
    }
    catch (const std::runtime_error&) {
    }
    EXPECT(!threw);
    EXPECT_THROWS_AS(SignalHandler::checkInterrupt(), Cancel);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
