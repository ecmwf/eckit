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
#include <thread>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

using fam::TestFam;


TestFam tester;

constexpr const auto num_threads = 8;
constexpr const auto list_size   = 200;
const auto list_name             = "L" + fam::random_number();
const auto list_data             = "D" + fam::random_number();

std::vector<std::string> test_data;
std::mutex test_mutex;

std::string makeTestData(const int number) {
    std::ostringstream oss;
    oss << "tid:" << std::this_thread::get_id() << " #" << number << '-' << list_data;
    auto value = oss.str();
    // add to the control list
    const std::lock_guard<std::mutex> lock(test_mutex);
    test_data.emplace_back(value);
    return value;
}

void populateList() {
    FamList list(tester.lastRegion(), list_name);
    for (auto i = 0; i < list_size; i++) {
        auto buffer = makeTestData(i);
        list.pushBack(buffer.data(), buffer.size());
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: create an empty list and validate size, empty, front, back") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto list_region = tester.makeRandomRegion(region_size);
    const auto list  = FamList(list_region, list_name);

    // empty list should have size 0
    EXPECT(list.empty());
    EXPECT(list.size() == 0);

    // front/back should throw as it's undefined behavior
    EXPECT_THROWS({ auto front = list.front(); });
    EXPECT_THROWS({ auto back = list.back(); });
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: pop on empty list throws") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto region =
        FamRegionName(fam::test_endpoint, "").withRegion("RP" + fam::random_number()).create(region_size, 0640, true);
    auto list = FamList(region, "LP" + fam::random_number());

    EXPECT(list.empty());
    EXPECT_EQUAL(list.size(), 0);

    EXPECT_THROWS({ list.popFront(); });
    EXPECT_THROWS({ list.popBack(); });

    region.destroy();
}

CASE("FamList: populate a list and validate size, !empty, front, back") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto region =
        FamRegionName(fam::test_endpoint, "").withRegion("LR" + fam::random_number()).create(region_size, 0640, true);
    auto list = FamList(region, list_name);

    // empty list should have size 0
    EXPECT(list.empty());
    EXPECT_EQUAL(list.size(), 0);

    std::string front = "Front FAM List!";
    EXPECT_NO_THROW(list.pushBack(front));
    EXPECT_EQUAL(list.size(), 1);
    EXPECT_EQUAL(list.front().view(), front);
    EXPECT_EQUAL(list.back().view(), front);


    std::string back = "Back FAM List!";
    EXPECT_NO_THROW(list.pushBack(back));
    EXPECT_EQUAL(list.size(), 2);
    EXPECT_EQUAL(list.front().view(), front);
    EXPECT_EQUAL(list.back().view(), back);

    region.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: pop front/back updates size and values") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto region =
        FamRegionName(fam::test_endpoint, "").withRegion("RP" + fam::random_number()).create(region_size, 0640, true);
    auto list = FamList(region, "LP" + fam::random_number());

    std::string first  = "first";
    std::string second = "second";
    std::string third  = "third";

    EXPECT_NO_THROW(list.pushBack(first));
    EXPECT_NO_THROW(list.pushBack(second));
    EXPECT_NO_THROW(list.pushBack(third));

    EXPECT_EQUAL(list.size(), 3);
    EXPECT_EQUAL(list.front().view(), first);
    EXPECT_EQUAL(list.back().view(), third);

    EXPECT_NO_THROW(list.popFront());
    EXPECT_EQUAL(list.size(), 2);
    EXPECT_EQUAL(list.front().view(), second);
    EXPECT_EQUAL(list.back().view(), third);

    EXPECT_NO_THROW(list.popBack());
    EXPECT_EQUAL(list.size(), 1);
    EXPECT_EQUAL(list.front().view(), second);
    EXPECT_EQUAL(list.back().view(), second);

    EXPECT_NO_THROW(list.popBack());
    EXPECT(list.empty());
    EXPECT_EQUAL(list.size(), 0);

    region.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: populate with " + std::to_string(list_size) + " items by " + std::to_string(num_threads) + " threads") {
    std::vector<std::thread> threads;

    test_data.reserve(num_threads * list_size);
    threads.reserve(num_threads);

    for (auto i = 0; i < num_threads; i++) {
        EXPECT_NO_THROW(threads.emplace_back(populateList));
    }

    for (auto&& thread : threads) {
        thread.join();
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: validate size and values after creation") {
    const auto list = FamList(tester.lastRegion(), list_name);

    EXPECT_NOT(list.empty());

    EXPECT(list.size() == num_threads * list_size);

    for (const auto& item : list) {
        // std::cout << "Validating item: " << item.view() << '\n';
        EXPECT(std::find(test_data.cbegin(), test_data.cend(), item.view()) != test_data.cend());
    }
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
