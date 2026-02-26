/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// Tests for FileSpaceStrategies using a mock filesystem registered via
/// PathNameFactory. No real filesystem state is accessed, so the tests
/// are fully deterministic.

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/BasePathName.h"
#include "eckit/filesystem/FileSpaceStrategies.h"
#include "eckit/filesystem/FileSystemSize.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/PathNameFactory.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/testing/Test.h"

#include <ctime>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// FakeFileSystem — mock satisfying the BasePathNameT<T> contract
//----------------------------------------------------------------------------------------------------------------------

enum class Availability {
    Available,
    Unavailable
};

enum class SizeQuery {
    Succeeds,
    Throws
};

struct FakeFileSystemConfig {
    Availability availability;
    FileSystemSize size;
    SizeQuery sizeQuery;
};

class FakeFileSystem {
public:

    // -- Static config registry --

    static std::map<std::string, FakeFileSystemConfig>& configs() {
        static std::map<std::string, FakeFileSystemConfig> m;
        return m;
    }

    // -- Constructors --

    FakeFileSystem() : path_("/fake") {}
    FakeFileSystem(const std::string& p, bool /*tildeIsUserHome*/ = false) : path_(p) {}
    FakeFileSystem(const char* p, bool /*tildeIsUserHome*/ = false) : path_(p) {}
    FakeFileSystem(const FakeFileSystem& other) : path_(other.path_) {}

    // -- Type identifier --

    static const char* type() { return "fake"; }

    // -- Conversion --

    operator std::string() const { return path_; }

    friend std::ostream& operator<<(std::ostream& s, const FakeFileSystem& f) {
        s << f.path_;
        return s;
    }

    // -- Required accessors --

    const std::string& path() const { return path_; }

    const std::string& node() const {
        static const std::string n = "localhost";
        return n;
    }

    // -- Methods used by strategies --

    bool available() const {
        auto it = configs().find(path_);
        if (it == configs().end()) {
            return false;
        }
        return it->second.availability == Availability::Available;
    }

    void fileSystemSize(FileSystemSize& fs) const {
        auto it = configs().find(path_);
        if (it == configs().end()) {
            throw Exception("FakeFileSystem: no config for " + path_);
        }
        if (it->second.sizeQuery == SizeQuery::Throws) {
            throw Exception("FakeFileSystem: simulated statvfs failure for " + path_);
        }
        fs = it->second.size;
    }

    // -- Stubs (not called by strategies) --

    const char* localPath() const { throw NotImplemented("FakeFileSystem::localPath", Here()); }
    Length size() const { throw NotImplemented("FakeFileSystem::size", Here()); }
    time_t lastAccess() const { throw NotImplemented("FakeFileSystem::lastAccess", Here()); }
    time_t lastModified() const { throw NotImplemented("FakeFileSystem::lastModified", Here()); }
    time_t created() const { throw NotImplemented("FakeFileSystem::created", Here()); }
    bool isDir() const { throw NotImplemented("FakeFileSystem::isDir", Here()); }
    bool isLink() const { throw NotImplemented("FakeFileSystem::isLink", Here()); }
    bool sameAs(const FakeFileSystem&) const { throw NotImplemented("FakeFileSystem::sameAs", Here()); }
    FakeFileSystem mountPoint() const { throw NotImplemented("FakeFileSystem::mountPoint", Here()); }
    FakeFileSystem realName() const { throw NotImplemented("FakeFileSystem::realName", Here()); }
    bool exists() const { throw NotImplemented("FakeFileSystem::exists", Here()); }
    bool offsite() const { throw NotImplemented("FakeFileSystem::offsite", Here()); }
    std::string clusterName() const { throw NotImplemented("FakeFileSystem::clusterName", Here()); }
    void mkdir(short) const { throw NotImplemented("FakeFileSystem::mkdir", Here()); }
    void chmod(short) const { throw NotImplemented("FakeFileSystem::chmod", Here()); }
    void unlink(bool) const { throw NotImplemented("FakeFileSystem::unlink", Here()); }
    void syncParentDirectory() const { throw NotImplemented("FakeFileSystem::syncParentDirectory", Here()); }
    void rmdir(bool) const { throw NotImplemented("FakeFileSystem::rmdir", Here()); }
    void touch() const { throw NotImplemented("FakeFileSystem::touch", Here()); }
    void children(std::vector<FakeFileSystem>&, std::vector<FakeFileSystem>&) const {
        throw NotImplemented("FakeFileSystem::children", Here());
    }
    void reserve(const Length&) const { throw NotImplemented("FakeFileSystem::reserve", Here()); }
    FakeFileSystem dirName() const { throw NotImplemented("FakeFileSystem::dirName", Here()); }
    FakeFileSystem orphanName() const { throw NotImplemented("FakeFileSystem::orphanName", Here()); }
    BasePathName* checkClusterNode() const { throw NotImplemented("FakeFileSystem::checkClusterNode", Here()); }
    FakeFileSystem fullName() const { throw NotImplemented("FakeFileSystem::fullName", Here()); }
    FakeFileSystem baseName(bool) const { throw NotImplemented("FakeFileSystem::baseName", Here()); }
    std::string extension() const { throw NotImplemented("FakeFileSystem::extension", Here()); }
    std::string hash(const std::string&) const { throw NotImplemented("FakeFileSystem::hash", Here()); }
    DataHandle* fileHandle(bool) const { throw NotImplemented("FakeFileSystem::fileHandle", Here()); }
    DataHandle* partHandle(const OffsetList&, const LengthList&) const {
        throw NotImplemented("FakeFileSystem::partHandle", Here());
    }
    DataHandle* partHandle(const Offset&, const Length&) const {
        throw NotImplemented("FakeFileSystem::partHandle", Here());
    }

    // -- Static stubs --

    static void rename(const FakeFileSystem&, const FakeFileSystem&) {
        throw NotImplemented("FakeFileSystem::rename", Here());
    }
    static void link(const FakeFileSystem&, const FakeFileSystem&) {
        throw NotImplemented("FakeFileSystem::link", Here());
    }
    static void match(const FakeFileSystem&, std::vector<FakeFileSystem>&, bool) {
        throw NotImplemented("FakeFileSystem::match", Here());
    }
    static FakeFileSystem unique(const FakeFileSystem&) { throw NotImplemented("FakeFileSystem::unique", Here()); }

private:

    std::string path_;
};

//----------------------------------------------------------------------------------------------------------------------
// Factory registration
//----------------------------------------------------------------------------------------------------------------------

static PathNameBuilder<FakeFileSystem> fakeBuilder("fake");

//----------------------------------------------------------------------------------------------------------------------
// RAII scope helper — clears config before and after each test
//----------------------------------------------------------------------------------------------------------------------

struct FakeFileSystemScope {
    FakeFileSystemScope() { FakeFileSystem::configs().clear(); }
    ~FakeFileSystemScope() { FakeFileSystem::configs().clear(); }
};

//----------------------------------------------------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------------------------------------------------

PathName fakePath(const std::string& name, unsigned long long avail, unsigned long long total,
                  Availability availability, SizeQuery sizeQuery) {
    FileSystemSize fs;
    fs.available                    = avail;
    fs.total                        = total;
    FakeFileSystem::configs()[name] = {availability, fs, sizeQuery};
    return PathName("fake", name);
}

PathName fakePath(const std::string& name, unsigned long long avail, unsigned long long total) {
    return fakePath(name, avail, total, Availability::Available, SizeQuery::Succeeds);
}

PathName fakeUnavailablePath(const std::string& name) {
    return fakePath(name, 0, 0, Availability::Unavailable, SizeQuery::Succeeds);
}

PathName fakeFailingPath(const std::string& name) {
    return fakePath(name, 0, 0, Availability::Available, SizeQuery::Throws);
}

//----------------------------------------------------------------------------------------------------------------------
// Test cases
//----------------------------------------------------------------------------------------------------------------------

CASE("leastUsed selects filesystem with most available bytes") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 100, 1000),
        fakePath("/fs2", 900, 1000),
        fakePath("/fs3", 500, 1000),
    };

    const PathName& result = FileSpaceStrategies::leastUsed(paths);
    EXPECT(result == "/fs2");
}

CASE("leastUsedPercent selects highest available percentage") {
    FakeFileSystemScope scope;

    // /fs1: 500/1000 = 50% free
    // /fs2: 200/10000 = 2% free (more absolute bytes, but lower %)
    // /fs3: 80/100 = 80% free
    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 200, 10000),
        fakePath("/fs3", 80, 100),
    };

    const PathName& result = FileSpaceStrategies::leastUsedPercent(paths);
    EXPECT(result == "/fs3");
}

CASE("leastUsed is deterministic") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 100, 1000),
        fakePath("/fs2", 900, 1000),
        fakePath("/fs3", 500, 1000),
    };

    for (int i = 0; i < 20; ++i) {
        const PathName& result = FileSpaceStrategies::leastUsed(paths);
        EXPECT(result == "/fs2");
    }
}

CASE("leastUsedPercent is deterministic") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 80, 100),
    };

    for (int i = 0; i < 20; ++i) {
        const PathName& result = FileSpaceStrategies::leastUsedPercent(paths);
        EXPECT(result == "/fs2");
    }
}

CASE("Unavailable filesystems are skipped") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakeUnavailablePath("/fs1"),
        fakePath("/fs2", 100, 1000),
    };

    const PathName& result = FileSpaceStrategies::leastUsed(paths);
    EXPECT(result == "/fs2");
}

CASE("fileSystemSize failure is tolerated") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakeFailingPath("/fs1"),
        fakePath("/fs2", 100, 1000),
    };

    const PathName& result = FileSpaceStrategies::leastUsed(paths);
    EXPECT(result == "/fs2");
}

CASE("All unavailable throws Retry") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakeUnavailablePath("/fs1"),
    };

    EXPECT_THROWS_AS(FileSpaceStrategies::leastUsed(paths), Retry);
}

CASE("Zero total size is skipped by leastUsedPercent") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 0, 0),
        fakePath("/fs2", 100, 1000),
    };

    const PathName& result = FileSpaceStrategies::leastUsedPercent(paths);
    EXPECT(result == "/fs2");
}

CASE("Filesystems over 99% used are excluded") {
    FakeFileSystemScope scope;

    // /fs1: 0/100 = 0% free → usedPercent=100 → excluded by findCandidates
    // /fs2: 200/1000 = 20% free → usedPercent=80 → included
    std::vector<PathName> paths = {
        fakePath("/fs1", 0, 100),
        fakePath("/fs2", 200, 1000),
    };

    const PathName& result = FileSpaceStrategies::pureRandom(paths);
    EXPECT(result == "/fs2");
}

CASE("All candidates filtered falls back to leastUsed") {
    FakeFileSystemScope scope;

    // Both 100% used → findCandidates returns empty → fallback to leastUsed
    std::vector<PathName> paths = {
        fakePath("/fs1", 0, 100),
        fakePath("/fs2", 0, 200),
    };

    // leastUsed picks any available path (both have 0 bytes free)
    const PathName& result = FileSpaceStrategies::pureRandom(paths);
    std::string s          = result.asString();
    EXPECT(s == "/fs1" || s == "/fs2");
}

CASE("roundRobin cycles through candidates in fixed repeating order") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 500, 1000),
        fakePath("/fs3", 500, 1000),
    };

    std::vector<std::string> results;
    for (int i = 0; i < 6; ++i) {
        results.push_back(FileSpaceStrategies::roundRobin(paths).asString());
    }

    // Consecutive calls cycle with period equal to candidate count
    EXPECT(results[0] == results[3]);
    EXPECT(results[1] == results[4]);
    EXPECT(results[2] == results[5]);

    // Consecutive calls within one cycle differ
    EXPECT(results[0] != results[1]);
    EXPECT(results[1] != results[2]);
}

CASE("pureRandom returns only valid candidates") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakeUnavailablePath("/fs2"),
    };

    for (int i = 0; i < 20; ++i) {
        const PathName& result = FileSpaceStrategies::pureRandom(paths);
        EXPECT(result == "/fs1");
    }
}

CASE("pureRandom eventually selects all candidates") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 500, 1000),
        fakePath("/fs3", 500, 1000),
    };

    // 3 uniform candidates: coupon-collector P(all seen in k) > 0.95 at k ~ 11.
    // Loop has no fixed bound — the test framework timeout is the failure mechanism.
    std::set<std::string> seen;
    while (seen.size() < 3) {
        seen.insert(FileSpaceStrategies::pureRandom(paths).asString());
    }
    // Loop exit guarantees all candidates were selected; timeout implies failure.
    EXPECT(true);
}

CASE("weightedRandom favours more available space") {
    FakeFileSystemScope scope;

    // /fs1: 900/1000 = 90% free
    // /fs2: 100/1000 = 10% free
    std::vector<PathName> paths = {
        fakePath("/fs1", 900, 1000),
        fakePath("/fs2", 100, 1000),
    };

    int fs1Count     = 0;
    const int trials = 1000;
    for (int i = 0; i < trials; ++i) {
        const PathName& result = FileSpaceStrategies::weightedRandom(paths);
        if (result == "/fs1") {
            fs1Count++;
        }
    }

    // With 9:1 weight ratio, /fs1 should be selected >50% of the time
    EXPECT(fs1Count > trials / 2);
}

CASE("weightedRandomPercent favours higher free percentage") {
    FakeFileSystemScope scope;

    // /fs1: 90/100 = 90% free (less absolute space)
    // /fs2: 100/1000 = 10% free (more absolute space)
    std::vector<PathName> paths = {
        fakePath("/fs1", 90, 100),
        fakePath("/fs2", 100, 1000),
    };

    int fs1Count     = 0;
    const int trials = 1000;
    for (int i = 0; i < trials; ++i) {
        const PathName& result = FileSpaceStrategies::weightedRandomPercent(paths);
        if (result == "/fs1") {
            fs1Count++;
        }
    }

    // With 90:10 percent ratio, /fs1 should be selected >50% of the time
    EXPECT(fs1Count > trials / 2);
}

CASE("binnedLeastUsed selects only from top bin") {
    FakeFileSystemScope scope;

    // /fs1 and /fs2 have similar high availability → same top bin
    // /fs3 has much less → lower bin
    // threshold = 900 - (900 - 100) / 20 = 860; /fs2 at 880 qualifies
    std::vector<PathName> paths = {
        fakePath("/fs1", 900, 1000),
        fakePath("/fs2", 880, 1000),
        fakePath("/fs3", 100, 1000),
    };

    std::set<std::string> seen;
    for (int i = 0; i < 100; ++i) {
        const PathName& result = FileSpaceStrategies::binnedLeastUsed(paths);
        seen.insert(result.asString());
    }

    EXPECT(seen.count("/fs1") > 0);
    EXPECT(seen.count("/fs2") > 0);
    EXPECT(seen.count("/fs3") == 0);
}

CASE("binnedLeastUsed with single candidate") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
    };

    const PathName& result = FileSpaceStrategies::binnedLeastUsed(paths);
    EXPECT(result == "/fs1");
}

CASE("selectFileSystem dispatches to named strategy") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 600, 1000),
    };

    const std::vector<std::string> strategies = {
        "leastUsed",      "leastUsedPercent",      "roundRobin",      "pureRandom",
        "weightedRandom", "weightedRandomPercent", "binnedLeastUsed",
    };

    for (const auto& strategy : strategies) {
        const PathName& result = FileSpaceStrategies::selectFileSystem(paths, strategy);
        std::string s          = result.asString();
        EXPECT(s == "/fs1" || s == "/fs2");
    }
}

CASE("Unknown strategy name falls back to leastUsed") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 100, 1000),
        fakePath("/fs2", 900, 1000),
    };

    std::string strategy   = "nonexistent";
    const PathName& result = FileSpaceStrategies::selectFileSystem(paths, strategy);
    EXPECT(result == "/fs2");
}

CASE("Single filesystem always selected") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
    };

    EXPECT(FileSpaceStrategies::leastUsed(paths) == "/fs1");
    EXPECT(FileSpaceStrategies::leastUsedPercent(paths) == "/fs1");
    EXPECT(FileSpaceStrategies::roundRobin(paths) == "/fs1");
    EXPECT(FileSpaceStrategies::pureRandom(paths) == "/fs1");
    EXPECT(FileSpaceStrategies::weightedRandom(paths) == "/fs1");
    EXPECT(FileSpaceStrategies::weightedRandomPercent(paths) == "/fs1");
    EXPECT(FileSpaceStrategies::binnedLeastUsed(paths) == "/fs1");
}

CASE("Empty vector asserts") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths;

    EXPECT_THROWS(FileSpaceStrategies::leastUsed(paths));
    EXPECT_THROWS(FileSpaceStrategies::leastUsedPercent(paths));
    EXPECT_THROWS(FileSpaceStrategies::roundRobin(paths));
    EXPECT_THROWS(FileSpaceStrategies::pureRandom(paths));
    EXPECT_THROWS(FileSpaceStrategies::weightedRandom(paths));
    EXPECT_THROWS(FileSpaceStrategies::weightedRandomPercent(paths));
    EXPECT_THROWS(FileSpaceStrategies::binnedLeastUsed(paths));
}

//----------------------------------------------------------------------------------------------------------------------
// Coverage gap tests
//----------------------------------------------------------------------------------------------------------------------

CASE("leastUsed with equal available bytes returns valid path") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 500, 1000),
    };

    const PathName& result = FileSpaceStrategies::leastUsed(paths);
    std::string s          = result.asString();
    EXPECT(s == "/fs1" || s == "/fs2");
}

CASE("leastUsedPercent with equal percentages returns valid path") {
    FakeFileSystemScope scope;

    // Both have 50% available
    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 500, 1000),
    };

    const PathName& result = FileSpaceStrategies::leastUsedPercent(paths);
    std::string s          = result.asString();
    EXPECT(s == "/fs1" || s == "/fs2");
}

CASE("leastUsedPercent all zero total throws Retry") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 0, 0),
        fakePath("/fs2", 0, 0),
    };

    EXPECT_THROWS_AS(FileSpaceStrategies::leastUsedPercent(paths), Retry);
}

CASE("Exactly 99% used passes candidate filter") {
    FakeFileSystemScope scope;

    // usedPercent = long(100.0 * (1000 - 10) / 1000) = long(99.0) = 99
    // 99 <= 99 should pass the filter
    std::vector<PathName> paths = {
        fakePath("/fs1", 10, 1000),
    };

    const PathName& result = FileSpaceStrategies::pureRandom(paths);
    EXPECT(result == "/fs1");
}

CASE("Zero total size skipped by findCandidates") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 0, 0),
        fakePath("/fs2", 500, 1000),
    };

    // pureRandom uses findCandidates; /fs1 should be skipped
    for (int i = 0; i < 20; ++i) {
        const PathName& result = FileSpaceStrategies::pureRandom(paths);
        EXPECT(result == "/fs2");
    }
}

CASE("binnedLeastUsed with all equal available selects all") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 500, 1000),
        fakePath("/fs2", 500, 1000),
        fakePath("/fs3", 500, 1000),
    };

    // When all equal: threshold = maxAvail - 0 = maxAvail, all qualify.
    // 3 uniform candidates: coupon-collector P(all seen in k) > 0.95 at k ~ 11.
    // Loop has no fixed bound — the test framework timeout is the failure mechanism.
    std::set<std::string> seen;
    while (seen.size() < 3) {
        seen.insert(FileSpaceStrategies::binnedLeastUsed(paths).asString());
    }
    // Loop exit guarantees all candidates were selected; timeout implies failure.
    EXPECT(true);
}

CASE("All fileSystemSize failures throws Retry") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakeFailingPath("/fs1"),
        fakeFailingPath("/fs2"),
    };

    EXPECT_THROWS_AS(FileSpaceStrategies::leastUsed(paths), Retry);
}

CASE("weightedRandom selects minority candidate") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 900, 1000),
        fakePath("/fs2", 100, 1000),
    };

    // 9:1 weights: P(minority unseen after k) = 0.9^k; P(both seen) > 0.95 at k ~ 29.
    // Loop has no fixed bound — the test framework timeout is the failure mechanism.
    std::set<std::string> seen;
    while (seen.size() < 2) {
        seen.insert(FileSpaceStrategies::weightedRandom(paths).asString());
    }
    // Loop exit guarantees all candidates were selected; timeout implies failure.
    EXPECT(true);
}

CASE("weightedRandomPercent selects minority candidate") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakePath("/fs1", 90, 100),
        fakePath("/fs2", 100, 1000),
    };

    // 9:1 weights: P(minority unseen after k) = 0.9^k; P(both seen) > 0.95 at k ~ 29.
    // Loop has no fixed bound — the test framework timeout is the failure mechanism.
    std::set<std::string> seen;
    while (seen.size() < 2) {
        seen.insert(FileSpaceStrategies::weightedRandomPercent(paths).asString());
    }
    // Loop exit guarantees all candidates were selected; timeout implies failure.
    EXPECT(true);
}

CASE("fileSystemSize failure tolerated by findCandidates-based strategy") {
    FakeFileSystemScope scope;

    std::vector<PathName> paths = {
        fakeFailingPath("/fs1"),
        fakePath("/fs2", 500, 1000),
    };

    // pureRandom goes through findCandidates, which has its own try/catch
    for (int i = 0; i < 20; ++i) {
        const PathName& result = FileSpaceStrategies::pureRandom(paths);
        EXPECT(result == "/fs2");
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
