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

/// @file   test_fam_multiprocess.cc
/// @author Metin Cakircali
/// @date   Apr 2026
///
/// @brief  Multi-process tests for FAM mock shared-memory backend.
///
/// Uses fork() to verify that multiple processes can concurrently
/// operate on the same FAM-resident data structures (FamList, FamMap)
/// through the process-shared POSIX shared-memory mock.

#include "test_fam_common.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamMap.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

fam::TestFam tester;

constexpr int num_children    = 4;
constexpr int items_per_child = 50;

/// Fork @p n child processes. Each child waits on a barrier, then runs @p fn(child_index).
/// Parent waits for all children to exit.
/// Returns true if every child exited with status 0.
template <typename Fn>
bool forkAndRun(int n, Fn&& fn) {
    // Pipe used as a barrier: children block on read until parent closes the write end.
    int barrier[2];
    if (::pipe(barrier) != 0) {
        return false;
    }

    std::vector<pid_t> pids;
    pids.reserve(n);

    for (int i = 0; i < n; ++i) {
        const pid_t pid = ::fork();
        if (pid < 0) {
            // fork failed — clean up already-forked children
            for (auto p : pids) {
                ::kill(p, SIGTERM);
                ::waitpid(p, nullptr, 0);
            }
            ::close(barrier[0]);
            ::close(barrier[1]);
            return false;
        }
        if (pid == 0) {
            // --- child ---
            ::close(barrier[1]);  // close write end
            char buf;
            // blocks until parent closes write end (unblocks all children simultaneously)
            static_cast<void>(::read(barrier[0], &buf, 1));
            ::close(barrier[0]);

            try {
                fn(i);
                ::_exit(0);  // no atexit handlers — parent owns cleanup
            }
            catch (...) {
                ::_exit(1);
            }
        }
        pids.push_back(pid);
    }

    // --- parent ---
    ::close(barrier[0]);
    ::close(barrier[1]);  // unblocks all children

    bool all_ok = true;
    for (auto pid : pids) {
        int status = 0;
        if (::waitpid(pid, &status, 0) < 0) {
            all_ok = false;
        }
        else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            all_ok = false;
        }
    }
    return all_ok;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("Multi-process FamList: concurrent pushBack from " + std::to_string(num_children) + " processes") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region    = tester.makeRandomRegion(region_size);
    auto list_name = "MPL" + fam::random_number();

    // Create the list in the parent so sentinels exist before forking.
    { FamList list(region, list_name); }

    bool ok = forkAndRun(num_children, [&](int child_id) {
        // Children inherit the mmap'd session — FamList reopens via idempotent sentinels.
        FamList list(region, list_name);
        for (int i = 0; i < items_per_child; ++i) {
            auto data = "c" + std::to_string(child_id) + "-i" + std::to_string(i);
            list.pushBack(data.data(), data.size());
        }
    });

    EXPECT(ok);

    // Parent validates that all items are present.
    FamList list(region, list_name);
    EXPECT_EQUAL(list.size(), num_children * items_per_child);

    // Collect all values and verify per-child items exist.
    std::set<std::string> found;
    for (const auto& item : list) {
        found.insert(std::string(item.view()));
    }
    EXPECT_EQUAL(found.size(), static_cast<std::size_t>(num_children * items_per_child));

    for (int c = 0; c < num_children; ++c) {
        for (int i = 0; i < items_per_child; ++i) {
            auto expected = "c" + std::to_string(c) + "-i" + std::to_string(i);
            EXPECT(found.count(expected) == 1);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Multi-process FamList: one writer, one reader") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;

    auto region         = tester.makeRandomRegion(region_size);
    auto list_name      = "MPWR" + fam::random_number();
    constexpr int count = 20;

    // Create the list before forking.
    { FamList list(region, list_name); }

    // Fork a single writer child — parent is the reader.
    int barrier[2];
    EXPECT(::pipe(barrier) == 0);

    const pid_t writer = ::fork();
    EXPECT(writer >= 0);

    if (writer == 0) {
        // --- writer child ---
        ::close(barrier[0]);
        try {
            FamList list(region, list_name);
            for (int i = 0; i < count; ++i) {
                auto data = "item-" + std::to_string(i);
                list.pushBack(data.data(), data.size());
            }
            // Signal completion by closing the pipe.
            ::close(barrier[1]);
            ::_exit(0);
        }
        catch (...) {
            ::close(barrier[1]);
            ::_exit(1);
        }
    }

    // --- parent (reader) ---
    ::close(barrier[1]);
    // Wait for writer to finish (pipe EOF).
    char buf;
    static_cast<void>(::read(barrier[0], &buf, 1));
    ::close(barrier[0]);

    int status = 0;
    ::waitpid(writer, &status, 0);
    EXPECT(WIFEXITED(status) && WEXITSTATUS(status) == 0);

    // Now read back what the child wrote.
    FamList list(region, list_name);
    EXPECT_EQUAL(list.size(), count);

    std::set<std::string> found;
    for (const auto& item : list) {
        found.insert(std::string(item.view()));
    }
    for (int i = 0; i < count; ++i) {
        EXPECT(found.count("item-" + std::to_string(i)) == 1);
    }
}

//----------------------------------------------------------------------------------------------------------------------

using FamMap32 = FamMap<FamMapEntry<32>>;

CASE("Multi-process FamMap: concurrent insert from " + std::to_string(num_children) + " processes") {
    constexpr eckit::fam::size_t region_size = 16 * 1024 * 1024;

    auto region   = tester.makeRandomRegion(region_size);
    auto map_name = "MPM" + fam::random_number();

    // Create the map in the parent.
    { FamMap32 map(map_name, region); }

    bool ok = forkAndRun(num_children, [&](int child_id) {
        FamMap32 map(map_name, region);
        for (int i = 0; i < items_per_child; ++i) {
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

    // Parent validates all entries.
    FamMap32 map(map_name, region);
    EXPECT_EQUAL(map.size(), static_cast<eckit::fam::size_t>(num_children * items_per_child));

    for (int c = 0; c < num_children; ++c) {
        for (int i = 0; i < items_per_child; ++i) {
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

CASE("Multi-process FamMap: one writer, one reader") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region         = tester.makeRandomRegion(region_size);
    auto map_name       = "MPKV" + fam::random_number();
    constexpr int count = 30;

    // Create the map before forking.
    { FamMap32 map(map_name, region); }

    int barrier[2];
    EXPECT(::pipe(barrier) == 0);

    const pid_t writer = ::fork();
    EXPECT(writer >= 0);

    if (writer == 0) {
        ::close(barrier[0]);
        try {
            FamMap32 map(map_name, region);
            for (int i = 0; i < count; ++i) {
                auto key_str = "wk-" + std::to_string(i);
                auto val_str = "wv-" + std::to_string(i);
                FamMap32::key_type key(key_str);
                map.insert(key, val_str);
            }
            ::close(barrier[1]);
            ::_exit(0);
        }
        catch (...) {
            ::close(barrier[1]);
            ::_exit(1);
        }
    }

    // --- parent (reader) ---
    ::close(barrier[1]);
    char buf;
    static_cast<void>(::read(barrier[0], &buf, 1));
    ::close(barrier[0]);

    int status = 0;
    ::waitpid(writer, &status, 0);
    EXPECT(WIFEXITED(status) && WEXITSTATUS(status) == 0);

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

CASE("Multi-process: idempotent region creation") {
    // Multiple processes try to create the same region — only one succeeds,
    // the rest get AlreadyExists and fall back to lookup.

    auto region_name                         = "MPRG" + fam::random_number();
    constexpr eckit::fam::size_t region_size = 1024 * 1024;

    bool ok = forkAndRun(num_children, [&](int /*child_id*/) {
        auto name = FamRegionName(fam::test_endpoint, "").withRegion(region_name);
        try {
            name.create(region_size, 0640);
        }
        catch (const AlreadyExists&) {
            // Expected for all but the first process.
            name.lookup();
        }
    });

    EXPECT(ok);

    // Parent verifies the region exists and is usable.
    auto name   = FamRegionName(fam::test_endpoint, "").withRegion(region_name);
    auto region = name.lookup();
    EXPECT(region.size() > 0);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Multi-process FamList: concurrent pushFront and pushBack") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;

    auto region    = tester.makeRandomRegion(region_size);
    auto list_name = "MPFB" + fam::random_number();

    { FamList list(region, list_name); }

    // Even-numbered children pushFront, odd-numbered pushBack.
    bool ok = forkAndRun(num_children, [&](int child_id) {
        FamList list(region, list_name);
        for (int i = 0; i < items_per_child; ++i) {
            auto data = "c" + std::to_string(child_id) + "-i" + std::to_string(i);
            if (child_id % 2 == 0) {
                list.pushFront(data.data(), data.size());
            }
            else {
                list.pushBack(data.data(), data.size());
            }
        }
    });

    EXPECT(ok);

    FamList list(region, list_name);
    EXPECT_EQUAL(list.size(), num_children * items_per_child);

    std::set<std::string> found;
    for (const auto& item : list) {
        found.insert(std::string(item.view()));
    }
    EXPECT_EQUAL(found.size(), static_cast<std::size_t>(num_children * items_per_child));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
