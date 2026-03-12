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
#include <thread>
#include <vector>

#include "eckit/io/fam/FamMap.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

constexpr std::size_t num_threads = 8;
constexpr std::size_t num_entries = 50;

}  // namespace

//----------------------------------------------------------------------------------------------------------------------
// Type aliases for the two key sizes under test

using FamMap32 = FamMap<FamMapEntry<32>>;
using FamMap64 = FamMap<FamMapEntry<64>>;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
// KeySize = 32
//
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: create empty map and validate size/empty") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("M" + fam::random_number(), region);

    EXPECT(map.empty());
    EXPECT_EQUAL(map.size(), 0);
    EXPECT_EQUAL(map.bucketCount(), 1024);
    EXPECT(map.begin() == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: insert single entry and find it") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MS" + fam::random_number(), region);

    FamMap32::key_type key("hello");
    std::string value = "world";

    auto [iter, inserted] = map.insert(key, value);

    EXPECT(inserted);
    EXPECT_EQUAL(map.size(), 1);
    EXPECT_NOT(map.empty());

    EXPECT(map.contains(key));
    auto found = map.find(key);
    EXPECT(found != map.end());

    auto entry = *found;
    EXPECT(entry.key == key);
    EXPECT_EQUAL(entry.value.view(), value);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: insert duplicate key returns false") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MD" + fam::random_number(), region);

    FamMap32::key_type key("duplicate");
    std::string val1 = "first";
    std::string val2 = "second";

    auto [it1, success1] = map.insert(key, val1);
    EXPECT(success1);
    EXPECT_EQUAL(map.size(), 1);

    auto [it2, success2] = map.insert(key, val2);
    EXPECT_NOT(success2);
    EXPECT_EQUAL(map.size(), 1);

    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), val1);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: find non-existent key returns end") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MM" + fam::random_number(), region);

    FamMap32::key_type key("ghost");

    EXPECT_NOT(map.contains(key));
    EXPECT(map.find(key) == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: insert multiple entries and iterate") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MI" + fam::random_number(), region);

    constexpr std::size_t count = 20;
    std::set<std::string> expected_keys;

    for (std::size_t i = 0; i < count; ++i) {
        auto key_str = "key-" + std::to_string(i);
        auto val_str = "val-" + std::to_string(i);
        FamMap32::key_type key(key_str);

        auto [iter, success] = map.insert(key, val_str);
        EXPECT(success);
        expected_keys.insert(key_str);
    }

    EXPECT_EQUAL(map.size(), count);

    std::set<std::string> found_keys;
    for (auto entry : map) {
        found_keys.insert(entry.key.asString());
        auto key_str      = entry.key.asString();
        auto expected_val = "val-" + key_str.substr(4);
        EXPECT_EQUAL(entry.value.view(), expected_val);
    }

    EXPECT_EQUAL(found_keys.size(), count);
    EXPECT(found_keys == expected_keys);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: erase existing key") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("ME" + fam::random_number(), region);

    FamMap32::key_type key1("alpha");
    FamMap32::key_type key2("beta");
    FamMap32::key_type key3("gamma");

    map.insert(key1, "a");
    map.insert(key2, "b");
    map.insert(key3, "c");
    EXPECT_EQUAL(map.size(), 3);

    auto erased = map.erase(key2);
    EXPECT_EQUAL(erased, 1);
    EXPECT_EQUAL(map.size(), 2);
    EXPECT_NOT(map.contains(key2));

    EXPECT(map.contains(key1));
    EXPECT(map.contains(key3));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: erase non-existent key returns 0") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MX" + fam::random_number(), region);

    FamMap32::key_type key("phantom");
    EXPECT_EQUAL(map.erase(key), 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: clear removes all entries") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MC" + fam::random_number(), region);

    for (std::size_t i = 0; i < 10; ++i) {
        FamMap32::key_type key("clr-" + std::to_string(i));
        map.insert(key, "data");
    }
    EXPECT_EQUAL(map.size(), 10);

    map.clear();
    EXPECT(map.empty());
    EXPECT_EQUAL(map.size(), 0);
    EXPECT(map.begin() == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: idempotent reopen preserves data") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "MR" + fam::random_number();

    FamMap32::key_type key("persist");
    std::string value = "survive";

    {
        auto map = FamMap32(name, region);
        map.insert(key, value);
        EXPECT_EQUAL(map.size(), 1);
    }

    {
        auto map = FamMap32(name, region);
        EXPECT_EQUAL(map.size(), 1);
        EXPECT(map.contains(key));
        auto entry = *map.find(key);
        EXPECT_EQUAL(entry.value.view(), value);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: concurrent insert from " + std::to_string(num_threads) + " threads with " +
     std::to_string(num_entries) + " entries each") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "MK" + fam::random_number();

    std::mutex data_mutex;
    std::set<std::string> all_keys;

    auto worker = [&](std::size_t thread_id) {
        auto map = FamMap32(name, region);
        for (std::size_t i = 0; i < num_entries; ++i) {
            auto key_str = "t" + std::to_string(thread_id) + "-k" + std::to_string(i);
            auto val_str = "v" + std::to_string(thread_id) + "-" + std::to_string(i);
            FamMap32::key_type key(key_str);

            auto [iter, success] = map.insert(key, val_str);
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

    auto map = FamMap32(name, region);
    EXPECT_EQUAL(map.size(), num_threads * num_entries);

    for (const auto& key_str : all_keys) {
        FamMap32::key_type key(key_str);
        EXPECT(map.contains(key));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: insert with empty value") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MV" + fam::random_number(), region);

    FamMap32::key_type key("no-value");
    auto [iter, success] = map.insert(key, nullptr, 0);
    EXPECT(success);
    EXPECT_EQUAL(map.size(), 1);

    auto entry = *map.find(key);
    EXPECT(entry.key == key);
    EXPECT_EQUAL(entry.value.size(), 0);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//
// KeySize = 64
//
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: create empty map and validate size/empty") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("N" + fam::random_number(), region);

    EXPECT(map.empty());
    EXPECT_EQUAL(map.size(), 0);
    EXPECT_EQUAL(map.bucketCount(), 1024);
    EXPECT_EQUAL(map.key_size, 64);
    EXPECT(map.begin() == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: insert single entry and find it") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("NS" + fam::random_number(), region);

    FamMap64::key_type key("long-key-for-64-byte-test");
    std::string value = "data-64";

    auto [iter, inserted] = map.insert(key, value);

    EXPECT(inserted);
    EXPECT_EQUAL(map.size(), 1);
    EXPECT_NOT(map.empty());

    EXPECT(map.contains(key));
    auto found = map.find(key);
    EXPECT(found != map.end());

    auto entry = *found;
    EXPECT(entry.key == key);
    EXPECT_EQUAL(entry.value.view(), value);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: insert duplicate key returns false") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("ND" + fam::random_number(), region);

    FamMap64::key_type key("dup-64-byte-key");
    std::string val1 = "first-64";
    std::string val2 = "second-64";

    auto [it1, success1] = map.insert(key, val1);
    EXPECT(success1);
    EXPECT_EQUAL(map.size(), 1);

    auto [it2, success2] = map.insert(key, val2);
    EXPECT_NOT(success2);
    EXPECT_EQUAL(map.size(), 1);

    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), val1);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: find non-existent key returns end") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("NM" + fam::random_number(), region);

    FamMap64::key_type key("ghost-64");

    EXPECT_NOT(map.contains(key));
    EXPECT(map.find(key) == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: insert multiple entries and iterate") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("NI" + fam::random_number(), region);

    constexpr std::size_t count = 20;
    std::set<std::string> expected_keys;

    for (std::size_t i = 0; i < count; ++i) {
        auto key_str = "key64-" + std::to_string(i);
        auto val_str = "val64-" + std::to_string(i);
        FamMap64::key_type key(key_str);

        auto [iter, success] = map.insert(key, val_str);
        EXPECT(success);
        expected_keys.insert(key_str);
    }

    EXPECT_EQUAL(map.size(), count);

    std::set<std::string> found_keys;
    for (auto entry : map) {
        found_keys.insert(entry.key.asString());
        auto key_str      = entry.key.asString();
        auto expected_val = "val64-" + key_str.substr(6);
        EXPECT_EQUAL(entry.value.view(), expected_val);
    }

    EXPECT_EQUAL(found_keys.size(), count);
    EXPECT(found_keys == expected_keys);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: erase existing key") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("NE" + fam::random_number(), region);

    FamMap64::key_type key1("alpha-64");
    FamMap64::key_type key2("beta-64");
    FamMap64::key_type key3("gamma-64");

    map.insert(key1, "a");
    map.insert(key2, "b");
    map.insert(key3, "c");
    EXPECT_EQUAL(map.size(), 3);

    auto erased = map.erase(key2);
    EXPECT_EQUAL(erased, 1);
    EXPECT_EQUAL(map.size(), 2);
    EXPECT_NOT(map.contains(key2));

    EXPECT(map.contains(key1));
    EXPECT(map.contains(key3));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: clear removes all entries") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("NC" + fam::random_number(), region);

    for (std::size_t i = 0; i < 10; ++i) {
        FamMap64::key_type key("clr64-" + std::to_string(i));
        map.insert(key, "data64");
    }
    EXPECT_EQUAL(map.size(), 10);

    map.clear();
    EXPECT(map.empty());
    EXPECT_EQUAL(map.size(), 0);
    EXPECT(map.begin() == map.end());
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: idempotent reopen preserves data") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "NR" + fam::random_number();

    FamMap64::key_type key("persist-64");
    std::string value = "survive-64";

    {
        auto map = FamMap64(name, region);
        map.insert(key, value);
        EXPECT_EQUAL(map.size(), 1);
    }

    {
        auto map = FamMap64(name, region);
        EXPECT_EQUAL(map.size(), 1);
        EXPECT(map.contains(key));
        auto entry = *map.find(key);
        EXPECT_EQUAL(entry.value.view(), value);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: concurrent insert from " + std::to_string(num_threads) + " threads with " +
     std::to_string(num_entries) + " entries each") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "NK" + fam::random_number();

    std::mutex data_mutex;
    std::set<std::string> all_keys;

    auto worker = [&](std::size_t thread_id) {
        auto map = FamMap64(name, region);
        for (std::size_t i = 0; i < num_entries; ++i) {
            auto key_str = "t" + std::to_string(thread_id) + "-k64-" + std::to_string(i);
            auto val_str = "v" + std::to_string(thread_id) + "-64-" + std::to_string(i);
            FamMap64::key_type key(key_str);

            auto [iter, success] = map.insert(key, val_str);
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

    auto map = FamMap64(name, region);
    EXPECT_EQUAL(map.size(), num_threads * num_entries);

    for (const auto& key_str : all_keys) {
        FamMap64::key_type key(key_str);
        EXPECT(map.contains(key));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<64>: insert with empty value") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap64("NV" + fam::random_number(), region);

    FamMap64::key_type key("no-value-64");
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
