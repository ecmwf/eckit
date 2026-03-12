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
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>

#include "eckit/io/fam/FamMap.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

constexpr std::size_t num_threads = 4;
constexpr std::size_t num_entries = 50;
const auto map_name               = "M" + fam::random_number();

static_assert(std::is_same_v<FamMap::iterator::value_type, FamMap::entry_type>,
              "FamMap iterator value_type must be FamMapEntry");

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: create empty map and validate size/empty") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap(map_name, region);

    EXPECT(map.empty());
    EXPECT_EQUAL(map.size(), 0);
    EXPECT_EQUAL(map.bucketCount(), 1024);
    EXPECT(map.begin() == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: insert single entry and find iter") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MS" + fam::random_number(), region);

    FamMap::key_type key("hello");
    std::string value = "world";

    auto [iter, inserted] = map.insert(key, std::string_view{value});

    EXPECT(inserted);
    EXPECT_EQUAL(map.size(), 1);
    EXPECT_NOT(map.empty());

    // find returns valid iterator
    EXPECT(map.contains(key));
    auto found = map.find(key);
    EXPECT(found != map.end());

    auto entry = *found;
    EXPECT(entry.key == key);
    EXPECT_EQUAL(entry.value.view(), value);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: insert duplicate key returns false") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MD" + fam::random_number(), region);

    FamMap::key_type key("duplicate");
    std::string val1 = "first";
    std::string val2 = "second";

    auto [it1, success1] = map.insert(key, std::string_view{val1});
    EXPECT(success1);
    EXPECT_EQUAL(map.size(), 1);

    auto [it2, success2] = map.insert(key, std::string_view{val2});
    EXPECT_NOT(success2);
    EXPECT_EQUAL(map.size(), 1);

    // Value should be the original (first insertion wins)
    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), val1);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: find non-existent key returns end") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MM" + fam::random_number(), region);

    FamMap::key_type key("ghost");

    EXPECT_NOT(map.contains(key));
    EXPECT(map.find(key) == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: insert multiple entries and iterate") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MI" + fam::random_number(), region);

    constexpr std::size_t count = 20;
    std::set<std::string> expected_keys;

    for (std::size_t i = 0; i < count; ++i) {
        auto key_str = "key-" + std::to_string(i);
        auto val_str = "val-" + std::to_string(i);
        FamMap::key_type key(key_str);

        auto [iter, success] = map.insert(key, std::string_view{val_str});
        EXPECT(success);
        expected_keys.insert(key_str);
    }

    EXPECT_EQUAL(map.size(), count);

    // Iterate and collect all keys
    std::set<std::string> found_keys;
    for (auto entry : map) {
        found_keys.insert(entry.key.asString());

        // Verify value matches
        auto key_str      = entry.key.asString();
        auto expected_val = "val-" + key_str.substr(4);  // strip "key-" prefix
        EXPECT_EQUAL(entry.value.view(), expected_val);
    }

    EXPECT_EQUAL(found_keys.size(), count);
    EXPECT(found_keys == expected_keys);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: erase existing key") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("ME" + fam::random_number(), region);

    FamMap::key_type key1("alpha");
    FamMap::key_type key2("beta");
    FamMap::key_type key3("gamma");

    map.insert(key1, std::string_view{"a"});
    map.insert(key2, std::string_view{"b"});
    map.insert(key3, std::string_view{"c"});
    EXPECT_EQUAL(map.size(), 3);

    // Erase middle entry
    auto erased = map.erase(key2);
    EXPECT_EQUAL(erased, 1);
    EXPECT_EQUAL(map.size(), 2);
    EXPECT_NOT(map.contains(key2));

    // Others still present
    EXPECT(map.contains(key1));
    EXPECT(map.contains(key3));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: erase non-existent key returns 0") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MX" + fam::random_number(), region);

    FamMap::key_type key("phantom");
    EXPECT_EQUAL(map.erase(key), 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: clear removes all entries") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MC" + fam::random_number(), region);

    for (std::size_t i = 0; i < 10; ++i) {
        FamMap::key_type key("clr-" + std::to_string(i));
        map.insert(key, std::string_view{"data"});
    }
    EXPECT_EQUAL(map.size(), 10);

    map.clear();
    EXPECT(map.empty());
    EXPECT_EQUAL(map.size(), 0);
    EXPECT(map.begin() == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: idempotent reopen preserves data") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region          = tester.makeRandomRegion(region_size);
    auto random_map_name = "MR" + fam::random_number();

    FamMap::key_type key("persist");
    std::string value = "survive";

    // First open: insert data
    {
        auto map = FamMap(random_map_name, region);
        map.insert(key, std::string_view{value});
        EXPECT_EQUAL(map.size(), 1);
    }

    // Second open: data should still be there
    {
        auto map = FamMap(random_map_name, region);
        EXPECT_EQUAL(map.size(), 1);
        EXPECT(map.contains(key));
        auto entry = *map.find(key);
        EXPECT_EQUAL(entry.value.view(), value);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: concurrent insert from " + std::to_string(num_threads) + " threads with " + std::to_string(num_entries) +
     " entries each") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "MK" + fam::random_number();

    std::mutex data_mutex;
    std::set<std::string> all_keys;

    auto worker = [&](std::size_t thread_id) {
        auto map = FamMap(name, region);  // idempotent reopen
        for (std::size_t i = 0; i < num_entries; ++i) {
            auto key_str = "t" + std::to_string(thread_id) + "-k" + std::to_string(i);
            auto val_str = "v" + std::to_string(thread_id) + "-" + std::to_string(i);
            FamMap::key_type key(key_str);

            auto [iter, success] = map.insert(key, std::string_view{val_str});
            EXPECT(success);

            const std::lock_guard lock(data_mutex);
            all_keys.insert(key_str);
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (std::size_t t = 0; t < num_threads; ++t) {
        threads.emplace_back(worker, t);
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify all entries present
    auto map = FamMap(name, region);
    EXPECT_EQUAL(map.size(), num_threads * num_entries);

    for (const auto& key_str : all_keys) {
        FamMap::key_type key(key_str);
        EXPECT(map.contains(key));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap: insert with empty value") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap("MV" + fam::random_number(), region);

    FamMap::key_type key("no-value");
    auto [iter, success] = map.insert(key, nullptr, 0);
    EXPECT(success);
    EXPECT_EQUAL(map.size(), 1);

    auto entry = *map.find(key);
    EXPECT(entry.key == key);
    EXPECT_EQUAL(entry.value.size(), 0);
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
