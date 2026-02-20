/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <set>
#include <string>
#include <vector>

#include "eckit/filesystem/FileSpaceStrategies.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

class TestFileSystems {
public:

    explicit TestFileSystems(size_t n) : base_() {
        for (size_t i = 0; i < n; ++i) {
            PathName dir(base_ / ("fs" + std::to_string(i)));
            dir.mkdir();
            fileSystems_.emplace_back(dir);
            validPaths_.insert(dir.asString());
        }
    }

    TestFileSystems(const TestFileSystems&)            = delete;
    TestFileSystems& operator=(const TestFileSystems&) = delete;

    const std::vector<PathName>& paths() const { return fileSystems_; }

    /// For membership checks
    const std::set<std::string>& validPaths() const { return validPaths_; }
    bool contains(const PathName& p) const { return validPaths_.count(p.asString()) == 1; }

private:

    TmpDir base_;
    std::vector<PathName> fileSystems_;
    std::set<std::string> validPaths_;
};

//----------------------------------------------------------------------------------------------------------------------

CASE("All strategies return a valid path from the input set") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();

    EXPECT(fixture.contains(FileSpaceStrategies::leastUsed(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::leastUsedPercent(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::roundRobin(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::pureRandom(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::weightedRandom(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::weightedRandomPercent(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::binnedLeastUsed(fs)));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("selectFileSystem dispatches to the named strategy") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();

    // Known strategy names
    std::vector<std::string> strategies = {
        "roundRobin",       "weightedRandom",  "pureRandom", "weightedRandomPercent",
        "leastUsedPercent", "binnedLeastUsed", "leastUsed" /* default / fallback */
    };

    for (const auto& s : strategies) {
        EXPECT(fixture.contains(FileSpaceStrategies::selectFileSystem(fs, s)));
    }

    // Unknown strategy name falls back to leastUsed
    EXPECT(fixture.contains(FileSpaceStrategies::selectFileSystem(fs, "unknownStrategy")));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Single filesystem always returned") {
    TestFileSystems fixture(1);
    const auto& fs = fixture.paths();

    EXPECT(FileSpaceStrategies::leastUsed(fs) == fs[0]);
    EXPECT(FileSpaceStrategies::leastUsedPercent(fs) == fs[0]);
    EXPECT(FileSpaceStrategies::roundRobin(fs) == fs[0]);
    EXPECT(FileSpaceStrategies::pureRandom(fs) == fs[0]);
    EXPECT(FileSpaceStrategies::weightedRandom(fs) == fs[0]);
    EXPECT(FileSpaceStrategies::weightedRandomPercent(fs) == fs[0]);
    EXPECT(FileSpaceStrategies::binnedLeastUsed(fs) == fs[0]);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Empty filesystem vector triggers assertion") {
    std::vector<PathName> empty;

    EXPECT_THROWS_AS(FileSpaceStrategies::leastUsed(empty), AssertionFailed);
    EXPECT_THROWS_AS(FileSpaceStrategies::leastUsedPercent(empty), AssertionFailed);
    EXPECT_THROWS_AS(FileSpaceStrategies::roundRobin(empty), AssertionFailed);
    EXPECT_THROWS_AS(FileSpaceStrategies::pureRandom(empty), AssertionFailed);
    EXPECT_THROWS_AS(FileSpaceStrategies::weightedRandom(empty), AssertionFailed);
    EXPECT_THROWS_AS(FileSpaceStrategies::weightedRandomPercent(empty), AssertionFailed);
    EXPECT_THROWS_AS(FileSpaceStrategies::binnedLeastUsed(empty), AssertionFailed);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("roundRobin cycles through candidates") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();

    std::vector<std::string> results;
    for (int i = 0; i < 6; ++i) {
        results.push_back(FileSpaceStrategies::roundRobin(fs).asString());
    }

    // Consecutive calls should cycle: result[i] == result[i+3] for a 3-element set
    EXPECT(results[0] == results[3]);
    EXPECT(results[1] == results[4]);
    EXPECT(results[2] == results[5]);

    // Consecutive calls should differ (with 3 candidates)
    EXPECT(results[0] != results[1]);
    EXPECT(results[1] != results[2]);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("leastUsed is deterministic when called repeatedly with same inputs") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();

    const PathName& r1 = FileSpaceStrategies::leastUsed(fs);
    const PathName& r2 = FileSpaceStrategies::leastUsed(fs);
    EXPECT(r1.asString() == r2.asString());
}

CASE("leastUsedPercent is deterministic when called repeatedly with same inputs") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();

    const PathName& r1 = FileSpaceStrategies::leastUsedPercent(fs);
    const PathName& r2 = FileSpaceStrategies::leastUsedPercent(fs);
    EXPECT(r1.asString() == r2.asString());
}

//----------------------------------------------------------------------------------------------------------------------
// Random strategies always return valid paths over many iterations

CASE("pureRandom returns valid paths over many iterations") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();
    std::set<PathName> returned;

    for (int i = 0; i < 50; ++i) {
        auto&& fs_returned = FileSpaceStrategies::pureRandom(fs);
        EXPECT(fixture.contains(fs_returned));
        returned.emplace(fs_returned);
    }

    EXPECT(returned.size() == 3);
}

CASE("weightedRandom returns valid paths over many iterations") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();
    std::set<PathName> returned;

    for (int i = 0; i < 50; ++i) {
        auto&& fs_returned = FileSpaceStrategies::weightedRandom(fs);
        EXPECT(fixture.contains(fs_returned));
        returned.emplace(fs_returned);
    }

    EXPECT(returned.size() == 3);
}

CASE("weightedRandomPercent returns valid paths over many iterations") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();
    std::set<PathName> returned;

    for (int i = 0; i < 50; ++i) {
        auto&& fs_returned = FileSpaceStrategies::weightedRandomPercent(fs);
        EXPECT(fixture.contains(fs_returned));
        returned.emplace(fs_returned);
    }

    EXPECT(returned.size() == 3);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Non-existent paths are skipped, valid ones still chosen") {
    TestFileSystems fixture(2);
    const auto& fs = fixture.paths();

    // Add a path that does not exist
    PathName badPath("/no/such/directory/ever");
    std::vector<PathName> mixed = {badPath, fs[0], fs[1]};

    EXPECT(fixture.contains(FileSpaceStrategies::leastUsed(mixed)));
    EXPECT(fixture.contains(FileSpaceStrategies::leastUsedPercent(mixed)));
    EXPECT(fixture.contains(FileSpaceStrategies::roundRobin(mixed)));
    EXPECT(fixture.contains(FileSpaceStrategies::pureRandom(mixed)));
    EXPECT(fixture.contains(FileSpaceStrategies::weightedRandom(mixed)));
    EXPECT(fixture.contains(FileSpaceStrategies::weightedRandomPercent(mixed)));
    EXPECT(fixture.contains(FileSpaceStrategies::binnedLeastUsed(mixed)));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("binnedLeastUsed returns valid paths over many iterations") {
    TestFileSystems fixture(3);
    const auto& fs = fixture.paths();
    std::set<PathName> returned;

    for (int i = 0; i < 50; ++i) {
        auto&& r = FileSpaceStrategies::binnedLeastUsed(fs);
        EXPECT(fixture.contains(r));
        returned.emplace(r);
    }

    // All directories are on the same device, so all have equal available space
    // and all land in the top bin — all should be chosen eventually
    EXPECT(returned.size() == 3);
}

CASE("binnedLeastUsed is deterministic with single candidate") {
    TestFileSystems fixture(1);
    const auto& fs = fixture.paths();

    for (int i = 0; i < 10; ++i) {
        EXPECT(FileSpaceStrategies::binnedLeastUsed(fs) == fs[0]);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Strategies work with many filesystem paths") {
    TestFileSystems fixture(10);
    const auto& fs = fixture.paths();

    EXPECT(fixture.contains(FileSpaceStrategies::leastUsed(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::roundRobin(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::weightedRandom(fs)));
    EXPECT(fixture.contains(FileSpaceStrategies::binnedLeastUsed(fs)));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
