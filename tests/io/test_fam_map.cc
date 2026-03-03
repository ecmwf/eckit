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

/// @file   test_fam_map.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <cstddef>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamMap.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

using fam::TestFam;

TestFam tester;

constexpr const std::size_t num_threads = 8;
constexpr const std::size_t num_maps    = 200;
const auto map_name                     = TestFam::makeRandomText("MAP");
const auto map_data                     = TestFam::makeRandomText("DATA");

// std::vector<std::string> test_data;
std::unordered_map<std::string, std::string> test_data;
std::mutex test_mutex;

auto makeTestData(const int number) -> std::string_view {
    std::ostringstream oss;
    oss << "tid:" << std::this_thread::get_id() << " #" << number << '-' << map_data;
    // add to the control list
    const std::lock_guard<std::mutex> lock(test_mutex);
    return test_data.emplace(oss.str(), oss.str()).first->second;
}

void populateMap() {
    // FamMap fam_map(tester.lastRegion(), map_name);
    // for (std::size_t i = 0; i < num_maps; i++) {
    // auto buffer = makeTestData(i);
    // EXPECT_NO_THROW(fam_map.push_back(buffer.data(), buffer.size()));
    // }
}


}  // namespace

#include <bits/hashtable_policy.h>

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: create an empty list and validate size, empty, front, back") {

    //     constexpr const auto region_size = 1024;
    //     auto map_region = tester.makeRandomRegion(region_size);
    //     const auto map = FamMap(map_region, map_name);
    //
    //     EXPECT(map.empty());
    //     EXPECT(map.size() == 0);
    //
    //     Buffer front;
    //     // EXPECT_NO_THROW(front = fam_map.front());
    //     EXPECT(front.size() == 0);
    //     EXPECT(front.data() == nullptr);
    //
    //     Buffer back;
    //     // EXPECT_NO_THROW(back = fam_map.back());
    //     EXPECT(back.size() == 0);
    //     EXPECT(back.data() == nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: populate with " + std::to_string(num_maps) + " items by " + std::to_string(num_threads) + " threads") {
    std::vector<std::thread> threads;

    threads.reserve(num_threads);

    for (std::size_t i = 0; i < num_threads; i++) {
        threads.emplace_back(populateMap);
    }

    for (auto&& thread : threads) {
        thread.join();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: validate size and values after creation") {
    // const auto fam_map = FamMap(tester.lastRegion(), map_name);
    // EXPECT_NOT(fam_map.empty());
    // EXPECT(fam_map.size() == num_threads * num_maps);
    // for (const auto& buffer : fam_map) {
    // EXPECT(std::find(testData.cbegin(), testData.cend(), buffer.view()) != testData.cend());
    // }
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
