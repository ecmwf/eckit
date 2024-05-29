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

#include <thread>
#include <vector>

using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

constexpr const auto regionName = "ECKIT_TEST_FAM_REGION";
constexpr const auto listName   = "ECKIT_TEST_FAM_CATALOGUE";
constexpr const auto listSize   = 100;

const FamConfig testConfig {{"127.0.0.1", 8880}, "EckitTestFAMSessionName"};

void createList() {
    FamRegion::SPtr region;

    EXPECT_NO_THROW(region = FamRegion::lookup(regionName, testConfig));

    FamList lst(region, listName);

    for (auto i = 0; i < listSize; i++) {
        std::ostringstream oss;
        oss << "[tid:" << std::this_thread::get_id() << "] THIS IS ECKIT TEST DATA #" << i;
        const auto& buffer = oss.str();
        lst.push_back(buffer.data(), buffer.size());
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: create concurrently") {
    std::vector<std::thread> threads;

    const size_t numThreads = 5;
    for (size_t i = 0; i < numThreads; i++) { threads.emplace_back(createList); }

    for (auto&& thread : threads) { thread.join(); }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    using namespace eckit;

    FamRegion::SPtr region = FamRegion::ensureCreated({1024, 0640, test::regionName}, test::testConfig);

    auto ret = run_tests(argc, argv);

    FamList lst(region, test::listName);
    for (const auto&& item : lst) { Log::info() << item.view() << '\n'; }

    // cleanup
    region->destroy();

    return ret;
}
