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

#include <mutex>
#include <thread>
#include <vector>

using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

constexpr const fam::size_t numThreads = 8;
constexpr const fam::size_t listSize   = 200;

constexpr const auto regionName = "ECKIT_TEST_FAM_REGION";
constexpr const auto listName   = "ECKIT_TEST_FAM_CATALOGUE";

const FamConfig testConfig {{"127.0.0.1", 8880}, "EckitTestFAMSessionName"};

std::vector<std::string> testData;
std::mutex               testMutex;

auto makeTestData(const int number) -> std::string {
    std::ostringstream oss;
    oss << "[tid:" << std::this_thread::get_id() << "] ECKIT TEST DATA #" << number;
    return oss.str();
}

void createList() {
    FamRegion::SPtr region;

    EXPECT_NO_THROW(region = FamRegion::lookup(regionName, testConfig));

    FamList lst(region, listName);

    for (auto i = 0; i < listSize; i++) {
        const auto buffer = makeTestData(i);
        // insert the buffer into the FAM list
        lst.push_back(buffer.data(), buffer.size());
        // insert the buffer into the control list
        const std::lock_guard<std::mutex> lock(testMutex);
        testData.emplace_back(buffer);
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: empty list") {
    FamRegion::SPtr region;

    EXPECT_NO_THROW(region = FamRegion::lookup(regionName, testConfig));

    const auto lst = FamList(region, listName);

    EXPECT(lst.empty());

    EXPECT(lst.size() == 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: create a list and insert " + std::to_string(listSize) + " items concurrently by " +
     std::to_string(numThreads) + " threads") {
    std::vector<std::thread> threads;

    threads.reserve(numThreads);

    for (auto i = 0; i < numThreads; i++) { threads.emplace_back(createList); }

    for (auto&& thread : threads) { thread.join(); }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: validate size and values after creation") {
    FamRegion::SPtr region;

    EXPECT_NO_THROW(region = FamRegion::lookup(regionName, testConfig));

    const auto lst = FamList(region, listName);

    EXPECT_NOT(lst.empty());

    EXPECT(lst.size() == numThreads * listSize);

    for (const auto& buffer : lst) {
        EXPECT(std::find(testData.cbegin(), testData.cend(), buffer.view()) != testData.cend());
    }
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    using namespace eckit;

    auto region = FamRegion::ensureCreated({1024, 0640, test::regionName}, test::testConfig);

    auto ret = run_tests(argc, argv);

    // cleanup
    region->destroy();

    return ret;
}
