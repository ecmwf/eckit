/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   test_log_threads_hammer.cc
/// @author Metin Cakircali
/// @date   Jun 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/log/Channel.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

#include <list>
#include <ostream>
#include <string>
#include <thread>

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

class SingletonTester {
    // comment out for worst case scenario
    // std::ostream& outDebug;
    // std::ostream& outDebugLib;
    // SingletonTester(): outDebug(Log::debug()), outDebugLib(Log::debug<LibEcKit>()) {
    SingletonTester() {
        Log::debug() << "--> SingletonTester()" << std::endl;
        Log::debug<LibEcKit>() << "--> SingletonTester(LIB)" << std::endl;
    }
    ~SingletonTester() {
        Log::debug() << "--> ~SingletonTester()" << std::endl;
        Log::debug<LibEcKit>() << "--> ~SingletonTester(LIB)" << std::endl;
    }

public:
    static auto instance() -> SingletonTester& {
        static SingletonTester tester;
        Log::info() << "SingletonTester::instance()" << std::endl;
        Log::warning() << "SingletonTester::instance()" << std::endl;
        Log::error() << "SingletonTester::instance()" << std::endl;
        Log::debug() << "SingletonTester::instance()" << std::endl;
        Log::debug<LibEcKit>() << "SingletonTester::instance()" << std::endl;
        return tester;
    }
};

auto& tester = SingletonTester::instance();

constexpr const auto numThreads = 32;
constexpr const auto logSize    = 1000;

auto makeTestLog(const std::string& channel, const int number) -> std::string {
    return "channel: " + channel + " #" + std::to_string(number) + '\n';
}

void runLoggers() {
    for (auto i = 0; i < logSize; i++) { Log::info() << makeTestLog("info-test", i); }
    for (auto i = 0; i < logSize; i++) { Log::warning() << makeTestLog("warn-test", i); }
    for (auto i = 0; i < logSize; i++) { Log::error() << makeTestLog("warn-test", i); }
    for (auto i = 0; i < logSize; i++) { Log::debug() << makeTestLog("debug-test", i); }
    for (auto i = 0; i < logSize; i++) { Log::debug<LibEcKit>() << makeTestLog("lib-debug-test", i); }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("Log: output " + std::to_string(logSize) + " logs by " + std::to_string(numThreads) + " threads") {
    std::list<std::thread> threads;

    for (auto i = 0; i < numThreads; i++) { EXPECT_NO_THROW(threads.emplace_back(runLoggers)); }

    for (auto&& thread : threads) { thread.join(); }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    auto count = 0;
    for (auto i = 0; i < 5; i++) { count += run_tests(argc, argv); }
    return count;
}
