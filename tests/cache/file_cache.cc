// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/FileCache.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/Roots.h"
#include "eckit/cache/loader/SharedMemory.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/Clock.h"


namespace eckit::cache::test {


constexpr int CONFIGURED_KEPT = 100;  // less than the times used below


// unique per test run (and process)
PathName unique_dir(const std::string& name) {
    static const auto dir = PathName::unique(PathName("eckit_cache/file_cache/run"));
    return dir / name;
}


std::string read(const PathName& path) {
    std::ifstream in(path.localPath(), std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}


void write(const PathName& path, const std::string& contents) {
    path.dirName().mkdir();
    std::ofstream(path.localPath(), std::ios::binary) << contents;
}


DiskCache::Write writing(const std::string& contents) {
    return [contents](const PathName& tmp) { write(tmp, contents); };
}


// permissions are not enforced for the super-user
bool permissions_enforced() {
    return ::geteuid() != 0;
}


CASE("FileCache: layout, created once and read-only") {
    const auto root = unique_dir("layout");
    const FileCache cache(Roots(root), "kind", 3, ".ext");

    EXPECT(cache.relative("key") == PathName("kind/3/key.ext"));

    EXPECT_NOT(cache.find("key"));

    const auto path = cache.getOrCreate("key", writing("contents"));
    EXPECT(path == root / "kind/3/key.ext");
    EXPECT(cache.find("key").value() == path);
    EXPECT(read(path) == "contents");

    // never modified, so read-only
    struct stat s{};
    EXPECT(::stat(path.localPath(), &s) == 0);
    EXPECT((s.st_mode & 0777) == DiskCache::PERMISSIONS);

    EXPECT(cache.getOrCreate("key", writing("other")) == path);
    EXPECT(read(path) == "contents");

    DiskCache::remove(root.dirName());
}


CASE("FileCache: found in a provisioned (read-only) cache, created in the writable") {
    if (!permissions_enforced()) {
        return;
    }

    const auto ro = unique_dir("read-only");
    const auto rw = unique_dir("read-write");
    const FileCache cache(Roots(std::vector<PathName>{ro, rw}), "kind", 1, "");

    write(ro / cache.relative("found"), "provisioned");
    write(ro / cache.relative("invalid"), "invalid");
    ::chmod(ro.localPath(), 0555);

    EXPECT(cache.getOrCreate("found", writing("x")) == ro / cache.relative("found"));
    EXPECT(read(ro / cache.relative("found")) == "provisioned");

    auto valid = [](const PathName& p) { return read(p) != "invalid"; };
    EXPECT(cache.getOrCreate("invalid", writing("valid"), valid) == rw / cache.relative("invalid"));
    EXPECT(read(rw / cache.relative("invalid")) == "valid");

    EXPECT(cache.getOrCreate("missing", writing("y")) == rw / cache.relative("missing"));
    EXPECT(read(rw / cache.relative("missing")) == "y");

    // nowhere to create
    const FileCache none(Roots(ro), "kind", 1, "");
    EXPECT_THROWS_AS(none.getOrCreate("missing", writing("z")), CacheError);

    ::chmod(ro.localPath(), 0755);
    DiskCache::remove(ro.dirName());
}


CASE("FileCache: the time recently used files are kept is configurable") {
    EXPECT(LibEcKitCache::fileCacheKept_s() == CONFIGURED_KEPT);
}


CASE("FileCache: within a capacity, the least recently used files are removed") {
    const auto root = unique_dir("capacity");
    const FileCache cache(Roots(root), "kind", 1, "", {}, false, 10);

    auto exists = [&](const std::string& key) { return (root / cache.relative(key)).exists(); };

    // last used some time ago (access and modification times)
    auto last_used = [&](const std::string& key, long seconds_ago) {
        const auto time = static_cast<long>(Clock::now()) - seconds_ago;
        const timeval times[2]{{time, 0}, {time, 0}};
        ASSERT(::utimes((root / cache.relative(key)).localPath(), times) == 0);
    };

    cache.getOrCreate("a", writing("aaaa"));
    cache.getOrCreate("b", writing("bbbb"));
    EXPECT(exists("a"));
    EXPECT(exists("b"));

    last_used("a", 300);
    last_used("b", 200);

    // over capacity: only as many as necessary, the least recently used first
    cache.getOrCreate("c", writing("cccc"));
    EXPECT_NOT(exists("a"));
    EXPECT(exists("b"));
    EXPECT(exists("c"));

    // finding a file is using it
    last_used("b", 300);
    last_used("c", 200);
    EXPECT(cache.find("b"));

    cache.getOrCreate("d", writing("dddd"));
    EXPECT(exists("b"));
    EXPECT_NOT(exists("c"));
    EXPECT(exists("d"));

    // recently used files are kept, even over capacity
    cache.getOrCreate("e", writing("eeee"));
    EXPECT(exists("b"));
    EXPECT(exists("d"));
    EXPECT(exists("e"));

    // removed files are created again
    EXPECT(read(cache.getOrCreate("a", writing("again"))) == "again");

    DiskCache::remove(root.dirName());
}


CASE("FileCache: loaded by the named loader, with the options of its kind") {
    const auto root = unique_dir("load");

    Loader::Options options;
    options.magic = 1;
    const FileCache cache(Roots(root), "kind", 1, "", options);

    const auto path = cache.getOrCreate("key", writing("contents"));

    const auto file = cache.load("file", path);
    EXPECT(std::string(static_cast<const char*>(file->address()), file->size()) == "contents");

    EXPECT_THROWS_AS(cache.load("unknown", path), CacheError);

    // the magic reached the loader: content in shared memory is not attached as another kind
    Loader::Options other_options;
    other_options.magic = 2;
    const FileCache other(Roots(root), "kind", 1, "", other_options);

    auto shared = cache.load("shared-memory", path);
    EXPECT_THROWS_AS(other.load("shared-memory", path), CacheError);

    shared.reset();
    loader::SharedMemory::unload(path);
    DiskCache::remove(root.dirName());
}


CASE("FileCache: throwing on cache miss") {
    const auto root = unique_dir("throw");
    // off by default (LibEcKitCache::throwOnMiss)
    EXPECT_NOT(LibEcKitCache::throwOnMiss());
    const FileCache cache(Roots(root), "kind", 1, "", {}, true);

    EXPECT_THROWS_AS(cache.getOrCreate("missing", writing("x")), CacheError);
    EXPECT_NOT((root / cache.relative("missing")).exists());

    // provisioned
    write(root / cache.relative("found"), "provisioned");
    EXPECT(read(cache.getOrCreate("found", writing("x"))) == "provisioned");

    DiskCache::remove(root.dirName());
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    // as a user would, read once
    ::setenv("ECKIT_CACHE_FILE_KEPT", std::to_string(eckit::cache::test::CONFIGURED_KEPT).c_str(), 1);

    return eckit::testing::run_tests(argc, argv);
}
