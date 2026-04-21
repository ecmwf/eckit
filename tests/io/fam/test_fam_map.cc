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
#include <set>
#include <string>

#include "eckit/io/fam/FamMap.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

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

CASE("FamMap<32>: insertOrAssign inserts new entry") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MO" + fam::random_number(), region);

    FamMap32::key_type key("assign-key");
    std::string val = "first";

    auto [iter, inserted] = map.insertOrAssign(key, val);
    EXPECT(inserted);
    EXPECT_EQUAL(map.size(), 1);

    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), val);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: insertOrAssign replaces existing entry") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MP" + fam::random_number(), region);

    FamMap32::key_type key("replace-me");
    std::string val1 = "original";
    std::string val2 = "replaced";

    auto [it1, ins1] = map.insertOrAssign(key, val1);
    EXPECT(ins1);
    EXPECT_EQUAL(map.size(), 1);

    auto [it2, ins2] = map.insertOrAssign(key, val2);
    EXPECT_NOT(ins2);  // replaced, not inserted
    EXPECT_EQUAL(map.size(), 1);

    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), val2);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: forceInsert allows duplicate keys") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MF" + fam::random_number(), region);

    FamMap32::key_type key("dup");
    std::string val1 = "first";
    std::string val2 = "second";

    map.forceInsert(key, val1);
    map.forceInsert(key, val2);

    EXPECT_EQUAL(map.size(), 2);
    EXPECT(map.contains(key));

    // find returns the most recently inserted (pushFront)
    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), val2);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: count returns correct number of entries per key") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MN" + fam::random_number(), region);

    FamMap32::key_type key("counted");
    FamMap32::key_type other("other");

    EXPECT_EQUAL(map.count(key), 0);

    map.forceInsert(key, "a");
    EXPECT_EQUAL(map.count(key), 1);

    map.forceInsert(key, "b");
    EXPECT_EQUAL(map.count(key), 2);

    map.forceInsert(other, "c");
    EXPECT_EQUAL(map.count(key), 2);
    EXPECT_EQUAL(map.count(other), 1);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: loadFactor reflects entry count") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("ML" + fam::random_number(), region);

    EXPECT(map.loadFactor() == 0.0f);

    for (std::size_t i = 0; i < 10; ++i) {
        FamMap32::key_type key("lf-" + std::to_string(i));
        map.insert(key, "v");
    }

    float expected = 10.0f / static_cast<float>(FamMap32::bucket_count);
    EXPECT(map.loadFactor() == expected);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: merge copies entries from another map") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map1   = FamMap32("MG1" + fam::random_number(), region);
    auto map2   = FamMap32("MG2" + fam::random_number(), region);

    FamMap32::key_type k1("alpha");
    FamMap32::key_type k2("beta");
    FamMap32::key_type k3("gamma");

    map1.insert(k1, "a1");
    map2.insert(k2, "b2");
    map2.insert(k3, "c3");

    map1.merge(map2);

    EXPECT_EQUAL(map1.size(), 3);
    EXPECT(map1.contains(k1));
    EXPECT(map1.contains(k2));
    EXPECT(map1.contains(k3));

    EXPECT_EQUAL((*map1.find(k2)).value.view(), "b2");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: merge skips existing keys") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map1   = FamMap32("MH1" + fam::random_number(), region);
    auto map2   = FamMap32("MH2" + fam::random_number(), region);

    FamMap32::key_type key("shared");

    map1.insert(key, "from-map1");
    map2.insert(key, "from-map2");

    map1.merge(map2);

    EXPECT_EQUAL(map1.size(), 1);
    EXPECT_EQUAL((*map1.find(key)).value.view(), "from-map1");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: erase after forceInsert removes all duplicates") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MQ" + fam::random_number(), region);

    FamMap32::key_type key("multi");

    map.forceInsert(key, "a");
    map.forceInsert(key, "b");
    map.forceInsert(key, "c");
    EXPECT_EQUAL(map.size(), 3);
    EXPECT_EQUAL(map.count(key), 3);

    auto erased = map.erase(key);
    EXPECT_EQUAL(erased, 3);
    EXPECT_EQUAL(map.size(), 0);
    EXPECT_NOT(map.contains(key));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: forceInsert entries are all visible during iteration") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MJ" + fam::random_number(), region);

    FamMap32::key_type key("multi");

    map.forceInsert(key, "val-a");
    map.forceInsert(key, "val-b");
    map.forceInsert(key, "val-c");
    EXPECT_EQUAL(map.size(), 3);

    // Iterate all entries — all three duplicates must appear.
    std::set<std::string> values;
    for (const auto& [k, v] : map) {
        EXPECT(k == key);
        values.insert(std::string{v.view()});
    }
    EXPECT_EQUAL(values.size(), 3);
    EXPECT(values.count("val-a"));
    EXPECT(values.count("val-b"));
    EXPECT(values.count("val-c"));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: insertOrAssign repeated replacement keeps size 1") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MW" + fam::random_number(), region);

    FamMap32::key_type key("hot-key");

    for (int i = 0; i < 5; ++i) {
        auto val            = "v" + std::to_string(i);
        auto [it, inserted] = map.insertOrAssign(key, val);
        // First iteration inserts, the rest replace.
        if (i == 0) {
            EXPECT(inserted);
        }
        else {
            EXPECT_NOT(inserted);
        }
        EXPECT_EQUAL(map.size(), 1);
    }

    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.view(), "v4");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: clear then re-insert works") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MY" + fam::random_number(), region);

    for (std::size_t i = 0; i < 5; ++i) {
        FamMap32::key_type key("phase1-" + std::to_string(i));
        map.insert(key, "data");
    }
    EXPECT_EQUAL(map.size(), 5);

    map.clear();
    EXPECT(map.empty());
    EXPECT(map.begin() == map.end());

    // Re-insert different keys.
    for (std::size_t i = 0; i < 3; ++i) {
        FamMap32::key_type key("phase2-" + std::to_string(i));
        auto [iter, success] = map.insert(key, "new");
        EXPECT(success);
    }
    EXPECT_EQUAL(map.size(), 3);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: move construction transfers ownership") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map1   = FamMap32("MZ" + fam::random_number(), region);

    FamMap32::key_type key("movable");
    map1.insert(key, "data");
    EXPECT_EQUAL(map1.size(), 1);

    auto map2 = std::move(map1);
    EXPECT_EQUAL(map2.size(), 1);
    EXPECT(map2.contains(key));
    EXPECT_EQUAL((*map2.find(key)).value.view(), "data");
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: operator<< prints summary") {
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MA" + fam::random_number(), region);

    map.insert(FamMap32::key_type("k1"), "v1");
    map.insert(FamMap32::key_type("k2"), "v2");

    std::ostringstream oss;
    oss << map;

    auto str = oss.str();
    // Verify the output contains expected substrings.
    EXPECT(str.find("FamMap") != std::string::npos);
    EXPECT(str.find("key_size=32") != std::string::npos);
    EXPECT(str.find("size=2") != std::string::npos);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: large binary values") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MB" + fam::random_number(), region);

    // Simulate FDB-like payload: a 512-byte binary blob.
    constexpr std::size_t payload_size = 512;
    std::vector<char> blob(payload_size);
    for (std::size_t i = 0; i < payload_size; ++i) {
        blob[i] = static_cast<char>(i % 256);
    }

    FamMap32::key_type key("big-val");
    auto [iter, success] = map.insert(key, blob.data(), payload_size);
    EXPECT(success);

    auto entry = *map.find(key);
    EXPECT_EQUAL(entry.value.size(), payload_size);
    EXPECT(std::memcmp(entry.value.data(), blob.data(), payload_size) == 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: cbegin/cend const iteration") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region = tester.makeRandomRegion(region_size);
    auto map    = FamMap32("MU" + fam::random_number(), region);

    map.insert(FamMap32::key_type("a"), "1");
    map.insert(FamMap32::key_type("b"), "2");

    std::size_t count = 0;
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        auto entry = *it;
        EXPECT(entry.value.size() > 0);
        ++count;
    }
    EXPECT_EQUAL(count, 2);
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

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: concurrent insert from 4 processes") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;
    constexpr int num_procs      = 4;
    constexpr int items_per_proc = 50;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPM" + fam::random_number();

    { FamMap32 map(map_name, region); }

    bool ok = forkAndRun(num_procs, [&](int child_id) {
        FamMap32 map(map_name, region);
        for (int i = 0; i < items_per_proc; ++i) {
            auto key_str = "p" + std::to_string(child_id) + "-k" + std::to_string(i);
            auto val_str = "v" + std::to_string(child_id) + "-" + std::to_string(i);
            FamMap32::key_type key(key_str);

            auto [iter, success] = map.insert(key, val_str);
            if (!success) {
                ::_exit(2);
            }
        }
    });

    EXPECT(ok);

    FamMap32 map(map_name, region);
    EXPECT_EQUAL(map.size(), static_cast<eckit::fam::size_t>(num_procs * items_per_proc));

    for (int c = 0; c < num_procs; ++c) {
        for (int i = 0; i < items_per_proc; ++i) {
            auto key_str = "p" + std::to_string(c) + "-k" + std::to_string(i);
            FamMap32::key_type key(key_str);
            EXPECT(map.contains(key));

            auto entry        = *map.find(key);
            auto expected_val = "v" + std::to_string(c) + "-" + std::to_string(i);
            EXPECT_EQUAL(entry.value.view(), expected_val);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: one writer process, parent reads") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;
    constexpr int count = 30;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPKV" + fam::random_number();

    { FamMap32 map(map_name, region); }

    bool ok = forkWriter([&]() {
        FamMap32 map(map_name, region);
        for (int i = 0; i < count; ++i) {
            auto key_str = "wk-" + std::to_string(i);
            auto val_str = "wv-" + std::to_string(i);
            FamMap32::key_type key(key_str);
            map.insert(key, val_str);
        }
    });

    EXPECT(ok);

    FamMap32 map(map_name, region);
    EXPECT_EQUAL(map.size(), static_cast<eckit::fam::size_t>(count));

    for (int i = 0; i < count; ++i) {
        auto key_str = "wk-" + std::to_string(i);
        FamMap32::key_type key(key_str);
        EXPECT(map.contains(key));

        auto entry = *map.find(key);
        EXPECT_EQUAL(entry.value.view(), "wv-" + std::to_string(i));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: lock/unlock serialises concurrent read-modify-write") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;
    constexpr int num_writers                = 4;
    constexpr int increments_per_proc        = 50;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPLK" + fam::random_number();

    // Pre-create map and seed the counter key with "0".
    {
        FamMap32 map(map_name, region);
        FamMap32::key_type key("counter");
        map.insert(key, "0");
    }

    bool ok = forkAndRun(num_writers, [&](int /*id*/) {
        FamMap32 map(map_name, region);
        FamMap32::key_type key("counter");

        for (int i = 0; i < increments_per_proc; ++i) {
            map.lock();
            auto entry = *map.find(key);
            int val    = std::stoi(std::string(entry.value.view()));
            map.insertOrAssign(key, std::to_string(val + 1));
            map.unlock();
        }
    });

    EXPECT(ok);

    FamMap32 map(map_name, region);
    FamMap32::key_type key("counter");
    auto entry       = *map.find(key);
    int final_val    = std::stoi(std::string(entry.value.view()));
    int expected_val = num_writers * increments_per_proc;
    eckit::Log::info() << "lock/unlock counter: " << final_val << "/" << expected_val << std::endl;
    EXPECT_EQUAL(final_val, expected_val);
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
