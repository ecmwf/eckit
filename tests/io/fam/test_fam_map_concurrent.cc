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
#include <cstdlib>
#include <set>
#include <string>

#include "eckit/io/fam/FamMap.h"
#include "eckit/io/fam/FamRegionName.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

fam::TestFam tester;

}  // namespace

//----------------------------------------------------------------------------------------------------------------------
// CHILD WORKER — runs in a clean exec'd process (no inherited gRPC state)

namespace {

/// Look up a pre-created region by name from the test FAM endpoint.
FamRegion lookupRegion(const std::string& region_name) {
    return FamRegionName(fam::test_endpoint, "").withRegion(region_name).lookup();
}

/// Worker entry point for "concurrent insert" test.
int worker_insert(int child_id, const std::vector<std::pair<std::string, std::string>>& args) {
    auto region   = lookupRegion(get_worker_arg(args, "region"));
    auto map_name = get_worker_arg(args, "map");
    int count     = std::stoi(get_worker_arg(args, "count"));

    FamMap32 map(map_name, region);
    for (int i = 0; i < count; ++i) {
        auto key_str = "p" + std::to_string(child_id) + "-k" + std::to_string(i);
        auto val_str = "v" + std::to_string(child_id) + "-" + std::to_string(i);
        FamMap32::key_type key(key_str);
        auto [iter, success] = map.insert(key, val_str);
        if (!success) {
            return 2;
        }
    }
    return 0;
}

/// Worker entry point for "one writer" test.
int worker_write(int /*child_id*/, const std::vector<std::pair<std::string, std::string>>& args) {
    auto region   = lookupRegion(get_worker_arg(args, "region"));
    auto map_name = get_worker_arg(args, "map");
    int count     = std::stoi(get_worker_arg(args, "count"));

    FamMap32 map(map_name, region);
    for (int i = 0; i < count; ++i) {
        auto key_str = "wk-" + std::to_string(i);
        auto val_str = "wv-" + std::to_string(i);
        FamMap32::key_type key(key_str);
        map.insert(key, val_str);
    }
    return 0;
}

/// Worker entry point for "lock/unlock" test.
int worker_lock(int /*child_id*/, const std::vector<std::pair<std::string, std::string>>& args) {
    auto region   = lookupRegion(get_worker_arg(args, "region"));
    auto map_name = get_worker_arg(args, "map");
    int count     = std::stoi(get_worker_arg(args, "count"));

    FamMap32 map(map_name, region);
    FamMap32::key_type key("counter");
    for (int i = 0; i < count; ++i) {
        map.lock();
        auto entry = *map.find(key);
        int val    = std::stoi(std::string(entry.value.view()));
        map.insertOrAssign(key, std::to_string(val + 1));
        map.unlock();
    }
    return 0;
}

/// Dispatch table for child workers.
int child_worker_main(int argc, char** argv) {
    auto args    = parse_worker_args(argc, argv);
    int child_id = std::stoi(get_worker_arg(args, "worker-id"));
    auto fn      = get_worker_arg(args, "fn");

    if (fn == "insert") {
        return worker_insert(child_id, args);
    }
    if (fn == "write") {
        return worker_write(child_id, args);
    }
    if (fn == "lock") {
        return worker_lock(child_id, args);
    }

    return 1;  // unknown worker
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------

CASE("FamMap<32>: concurrent insert from 4 processes") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;
    constexpr int num_procs                  = 4;
    constexpr int items_per_proc             = 50;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPM" + fam::random_number();

    { FamMap32 map(map_name, region); }

    bool ok = fork_and_exec(num_procs, {
                                           "--fn=insert",
                                           "--region=" + region.name(),
                                           "--map=" + map_name,
                                           "--count=" + std::to_string(items_per_proc),
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

    bool ok = fork_and_exec(1, {
                                   "--fn=write",
                                   "--region=" + region.name(),
                                   "--map=" + map_name,
                                   "--count=" + std::to_string(count),
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

    bool ok = fork_and_exec(num_writers, {
                                             "--fn=lock",
                                             "--region=" + region.name(),
                                             "--map=" + map_name,
                                             "--count=" + std::to_string(increments_per_proc),
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
    auto args = eckit::testing::parse_worker_args(argc, argv);
    if (!args.empty()) {
        eckit::Main::initialise(argc, argv);
        return eckit::test::child_worker_main(argc, argv);
    }
    return eckit::testing::run_tests(argc, argv);
}
