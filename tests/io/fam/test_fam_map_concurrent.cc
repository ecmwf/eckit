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

/// @file   test_fam_map_concurrent.cc
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

using FamMap32 = FamMap<FamMapEntry<32>>;

//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: concurrent insert from 4 processes") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;
    constexpr int num_procs                  = 4;
    constexpr int items_per_proc             = 50;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPM" + fam::random_number();

    { FamMap32 map(map_name, region); }

    bool ok = fork_and_run(num_procs, [&](int child_id) {
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
    constexpr int count                      = 30;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPKV" + fam::random_number();

    { FamMap32 map(map_name, region); }

    bool ok = fork_and_run(1, [&](int) {
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

    bool ok = fork_and_run(num_writers, [&](int /*id*/) {
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
