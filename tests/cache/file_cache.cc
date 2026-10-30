// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/FileCache.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/Roots.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


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
    return eckit::testing::run_tests(argc, argv);
}
