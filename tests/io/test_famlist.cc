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

#include "test_fam_common.h"

#include <algorithm>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

constexpr const auto num_threads = 8;
constexpr const auto list_size   = 200;
const auto list_name             = fam::TestFam::makeRandomText("LIST");
const auto list_data             = fam::TestFam::makeRandomText("DATA");

std::vector<std::string> test_data;
std::mutex test_mutex;

auto makeTestData(const int number) -> std::string_view {
    std::ostringstream oss;
    oss << "tid:" << std::this_thread::get_id() << " #" << number << '-' << list_data;
    // add to the control list
    const std::lock_guard<std::mutex> lock(test_mutex);
    return test_data.emplace_back(oss.str());
}

void populateList() {
    FamList lst(tester.lastRegion(), list_name);
    for (auto i = 0; i < list_size; i++) {
        auto buffer = makeTestData(i);
        EXPECT_NO_THROW(lst.pushBack(buffer.data(), buffer.size()));
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: create an empty list and validate size, empty, front, back") {
    auto list_region = tester.makeRandomRegion(1024);

    const auto lst = FamList(list_region, list_name);

    EXPECT(lst.empty());

    EXPECT(lst.size() == 0);

    Buffer front;
    EXPECT_NO_THROW(front = lst.front());
    EXPECT(front.size() == 0);
    EXPECT(front.data() == nullptr);

    Buffer back;
    EXPECT_NO_THROW(back = lst.back());
    EXPECT(back.size() == 0);
    EXPECT(back.data() == nullptr);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: populate with " + std::to_string(list_size) + " items by " + std::to_string(num_threads) + " threads") {
    std::vector<std::thread> threads;

    threads.reserve(num_threads);

    for (auto i = 0; i < num_threads; i++) {
        threads.emplace_back(populateList);
    }

    for (auto&& thread : threads) {
        thread.join();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: validate size and values after creation") {
    const auto lst = FamList(tester.lastRegion(), list_name);

    EXPECT_NOT(lst.empty());

    EXPECT(lst.size() == num_threads * list_size);

    for (const auto& buffer : lst) {
        EXPECT(std::find(test_data.cbegin(), test_data.cend(), buffer.view()) != test_data.cend());
    }
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
