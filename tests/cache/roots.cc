// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Download.h"
#include "eckit/cache/Exceptions.h"
#include "eckit/cache/Roots.h"
#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/MD5.h"


namespace eckit::cache::test {


// unique per test run (and process)
PathName unique_dir(const std::string& name) {
    static const auto dir = PathName::unique(PathName("eckit_cache/roots/run"));
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


// permissions are not enforced for the super-user
bool permissions_enforced() {
    return ::geteuid() != 0;
}


CASE("Roots: a list, expanded") {
    const auto a = unique_dir("a");
    const auto b = unique_dir("b");

    SECTION("list, in order, empty entries ignored") {
        Roots roots(a.asString() + "::" + b.asString() + ":");
        EXPECT(roots.paths().size() == 2);
        EXPECT(roots.paths()[0] == a);
        EXPECT(roots.paths()[1] == b);

        // subdirectories
        auto sub = roots / "sub";
        EXPECT(sub.paths()[0] == a / "sub");
        EXPECT(sub.paths()[1] == b / "sub");
    }

    SECTION("environment variable") {
        ::setenv("ECKIT_TEST_CACHE_ROOT", b.asString().c_str(), 1);
        Roots roots(a.asString() + ":{ENVVAR?ECKIT_TEST_CACHE_ROOT}/c");
        EXPECT(roots.paths().size() == 2);
        EXPECT(roots.paths()[1] == b / "c");
    }

    SECTION("file contents (its first line)") {
        const auto file = unique_dir("root.txt");
        write(file, b.asString() + "\nignored\n");

        Roots roots("{FILE?" + file.asString() + "}/d");
        EXPECT(roots.paths().size() == 1);
        EXPECT(roots.paths()[0] == b / "d");
    }

    SECTION("the user's home") {
        const auto* home = ::getenv("HOME");
        if (home != nullptr) {
            Roots roots("~/x");
            EXPECT(roots.paths()[0] == PathName(home) / "x");
        }
    }

    DiskCache::remove(a.dirName());
}


CASE("Roots: found in order, written to the first writable") {
    const auto a = unique_dir("first");
    const auto b = unique_dir("second");
    const Roots roots(std::vector<PathName>{a, b});

    // found in order
    write(b / "file", "b");
    PathName found;
    EXPECT(roots.find("file", found) && found == b / "file");

    write(a / "file", "a");
    EXPECT(roots.find("file", found) && found == a / "file");

    // unless not valid
    EXPECT(roots.find("file", found, [](const PathName& p) { return read(p) == "b"; }) && found == b / "file");
    EXPECT_NOT(roots.find("missing", found));

    // writable, in order
    EXPECT(roots.writable_paths() == (std::vector<PathName>{a, b}));

    if (permissions_enforced()) {
        ::chmod(a.localPath(), 0555);
        EXPECT(roots.writable_paths() == std::vector<PathName>{b});
        EXPECT(Roots(std::vector<PathName>{a}).writable_paths().empty());
        ::chmod(a.localPath(), 0755);
    }

    // a cache directory, shared by all users
    const auto c = unique_dir("created");
    Roots::create_directory(c, "sub/directory");
    EXPECT((c / "sub/directory").isDir());

    // writable by all users: the root and under it
    for (const auto& dir : {c, c / "sub", c / "sub/directory"}) {
        struct stat st{};
        EXPECT(::stat(dir.localPath(), &st) == 0 && (st.st_mode & 0777) == 0777);
    }

    DiskCache::remove(a.dirName());
}


CASE("Roots: readable and writable paths, as they are when asked") {
    const auto a       = unique_dir("existing");
    const auto b       = unique_dir("creatable");
    const auto missing = PathName{"/dev/null/not-a-directory"};
    a.mkdir();

    const Roots roots(std::vector<PathName>{a, b, missing});

    // existing: searched for files
    EXPECT(roots.readable_paths() == std::vector<PathName>{a});

    // writable, or creatable (not created by asking)
    EXPECT(roots.writable_paths() == (std::vector<PathName>{a, b}));
    EXPECT_NOT(b.exists());

    // writing disabled: a read-only cache
    const Roots read_only(std::vector<PathName>{a, b}, false);
    EXPECT(read_only.readable_paths() == std::vector<PathName>{a});
    EXPECT(read_only.writable_paths().empty());

    EXPECT((read_only / "sub").writable_paths().empty());

    // as directories are created
    b.mkdir();
    EXPECT(roots.readable_paths() == (std::vector<PathName>{a, b}));

    DiskCache::remove(a.dirName());
}


#if eckit_HAVE_CURL
CASE("Download: found in any root, downloaded to the first writable") {
    const auto a = unique_dir("download-a");
    const auto b = unique_dir("download-b");

    // never downloaded, found in the second root
    const std::string url = "file:///does/not/exist/eckit_test_cache_roots";
    write(b / (MD5{url}.digest() + ".download"), "cached");

    const Download download(Roots(std::vector<PathName>{a, b}));
    const auto path = download.to_cached_path(url, "", ".download");
    EXPECT(path == b / (MD5{url}.digest() + ".download"));
    EXPECT(read(path) == "cached");

    // downloaded to the first (writable) root
    const auto source = unique_dir("download-source");
    write(source, "source");

    const auto url_source = "file://" + source.fullName().asString();
    EXPECT(download.to_cached_path(url_source).dirName() == a);

    DiskCache::remove(a.dirName());
}
#endif


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
