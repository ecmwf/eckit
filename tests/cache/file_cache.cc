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

    bool created    = false;
    const auto path = cache.get_or_create("key", writing("contents"), {}, &created);
    EXPECT(created);
    EXPECT(path == root / "kind/3/key.ext");
    EXPECT(read(path) == "contents");

    // never modified, so read-only
    struct stat s{};
    EXPECT(::stat(path.localPath(), &s) == 0);
    EXPECT((s.st_mode & 0777) == 0444);

    EXPECT(cache.get_or_create("key", writing("other"), {}, &created) == path);
    EXPECT_NOT(created);

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

    bool created = true;
    EXPECT(cache.get_or_create("found", writing("x"), {}, &created) == ro / cache.relative("found"));
    EXPECT_NOT(created);

    auto valid = [](const PathName& p) { return read(p) != "invalid"; };
    EXPECT(cache.get_or_create("invalid", writing("valid"), valid, &created) == rw / cache.relative("invalid"));
    EXPECT(created);

    EXPECT(cache.get_or_create("missing", writing("y"), {}, &created) == rw / cache.relative("missing"));
    EXPECT(created);

    // nowhere to create
    const FileCache none(Roots(ro), "kind", 1, "");
    EXPECT_THROWS_AS(none.get_or_create("missing", writing("z")), CacheError);

    ::chmod(ro.localPath(), 0755);
    DiskCache::remove(ro.dirName());
}


CASE("FileCache: throwing on cache miss") {
    const auto root = unique_dir("throw");
    FileCache::Options options;
    options.throw_on_miss = true;
    const FileCache cache(Roots(root), "kind", 1, "", options);

    EXPECT_THROWS_AS(cache.get_or_create("missing", writing("x")), CacheError);
    EXPECT_NOT((root / cache.relative("missing")).exists());

    // provisioned
    write(root / cache.relative("found"), "provisioned");
    EXPECT(read(cache.get_or_create("found", writing("x"))) == "provisioned");

    DiskCache::remove(root.dirName());
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
