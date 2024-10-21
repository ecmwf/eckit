/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   test_famlist.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "eckit/config/LibEcKit.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamConfig.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/testing/Test.h"
#include "fam_common.h"

#include <mutex>
#include <thread>
#include <vector>

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

constexpr const auto numThreads = 8;
constexpr const auto listSize   = 200;
const auto           listName   = test::fam::TestFam::makeRandomText("LIST");
const auto           listData   = test::fam::TestFam::makeRandomText("DATA");

std::vector<std::string> testData;
std::mutex               testMutex;

auto makeTestData(const int number) -> std::string_view {
    std::ostringstream oss;
    oss << "tid:" << std::this_thread::get_id() << " #" << number << '-' << listData;
    // add to the control list
    const std::lock_guard<std::mutex> lock(testMutex);
    return testData.emplace_back(oss.str());
}

void populateList() {
    FamList lst(*tester.getLastRegion(), listName);
    for (auto i = 0; i < listSize; i++) {
        auto buffer = makeTestData(i);
        EXPECT_NO_THROW(lst.push_back(buffer.data(), buffer.size()));
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: create an empty list and validate size, empty, front, back") {
    auto listRegion = tester.makeRandomRegion(1024);

    const auto lst = FamList(*listRegion, listName);

    EXPECT(lst.empty());

    EXPECT_EQUAL(lst.size(), 0);

    Buffer front;
    EXPECT_NO_THROW(front = lst.front());
    EXPECT_EQUAL(front.size(), 0);
    /// @todo remove comment once Buffer fixed
    // EXPECT_EQUAL(front.data(), nullptr);

    Buffer back;
    EXPECT_NO_THROW(back = lst.back());
    EXPECT_EQUAL(back.size(), 0);
    /// @todo remove comment once Buffer fixed
    // EXPECT_EQUAL(back.data(), nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: populate with " + std::to_string(listSize) + " items by " + std::to_string(numThreads) + " threads") {
    std::vector<std::thread> threads;

    threads.reserve(numThreads);

    for (auto i = 0; i < numThreads; i++) { threads.emplace_back(populateList); }

    for (auto&& thread : threads) { thread.join(); }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: validate size and values after creation") {
    const auto lst = FamList(*tester.getLastRegion(), listName);

    EXPECT_NOT(lst.empty());

    EXPECT_EQUAL(lst.size(), numThreads * listSize);

    for (const auto& buffer : lst) {
        EXPECT(std::find(testData.cbegin(), testData.cend(), buffer.view()) != testData.cend());
    }
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
