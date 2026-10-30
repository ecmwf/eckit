// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "eckit/cache/Unzip.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/StringTools.h"

#include "zip.h"


namespace eckit::cache::test {


/// Entries of a zip archive: name (directories end in '/') and contents
using Entries = std::map<std::string, std::string>;


const Entries ENTRIES{
    {"a", ""},                                    // empty
    {"b/", ""},                                   // directory
    {"b/c", "d\n"},                               // text
    {"e/f/g.bin", std::string("\0\1\2\377", 4)},  // binary, in directories without an entry of their own
};


/// A zip archive of the entries, made with libzip
PathName make_zip(const PathName& path, const Entries& entries) {
    int error = 0;
    auto* z   = zip_open(path.localPath(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    ASSERT(z != nullptr);

    for (const auto& [name, contents] : entries) {
        if (name.back() == '/') {
            ASSERT(zip_dir_add(z, name.c_str(), 0) >= 0);
            continue;
        }

        auto* source = zip_source_buffer(z, contents.data(), contents.size(), 0);
        ASSERT(source != nullptr);
        ASSERT(zip_file_add(z, name.c_str(), source, 0) >= 0);
    }

    ASSERT(zip_close(z) == 0);  // writes the archive
    return path;
}


void write_file(const PathName& path, const std::string& contents) {
    std::ofstream(path.localPath(), std::ios::binary) << contents;
    ASSERT(path.exists());
}


std::string read_file(const PathName& path) {
    std::ifstream in(path.localPath(), std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}


/// Files in a cache directory, except lock files (left in place, see eckit::cache::PathLock)
std::vector<PathName> files_without_locks(const PathName& dir) {
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    if (dir.exists()) {
        dir.children(files, dirs);
    }

    files.erase(std::remove_if(files.begin(), files.end(), [](const auto& f) { return f.extension() == ".lock"; }),
                files.end());
    return files;
}


CASE("list") {
    const auto zip = make_zip("test.unzip.list.zip", ENTRIES);

    EXPECT(Unzip::list(zip) == std::vector<std::string>({"a", "b/c", "e/f/g.bin"}));
    EXPECT(Unzip::list(zip, false) == std::vector<std::string>({"a", "b/", "b/c", "e/f/g.bin"}));

    zip.unlink();
}


CASE("to_path: all files, into a directory") {
    const auto zip = make_zip("test.unzip.all.zip", ENTRIES);
    const PathName dir("test.unzip.all");
    DiskCache::remove(dir);

    Unzip::to_path(zip, dir);

    for (const auto& [name, contents] : ENTRIES) {
        const auto path = dir / name;
        EXPECT(name.back() == '/' ? path.isDir() : read_file(path) == contents);
    }

    DiskCache::remove(dir);
    zip.unlink();
}


CASE("to_path: an entry, into a file") {
    const auto zip = make_zip("test.unzip.one.zip", ENTRIES);
    const PathName dir("test.unzip.one");
    DiskCache::remove(dir);

    Unzip::to_path(zip, dir / "file", "b/c");
    EXPECT(read_file(dir / "file") == "d\n");

    // and nothing else
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    dir.children(files, dirs);
    EXPECT(files.size() == 1 && dirs.empty());

    // a missing entry is an error
    EXPECT_THROWS_AS(Unzip::to_path(zip, dir / "missing", "no/such/entry"), ReadError);
    EXPECT_NOT((dir / "missing").exists());

    DiskCache::remove(dir);
    zip.unlink();
}


CASE("to_path, to_cached_path: an archive without files is an empty directory") {
    const auto zip = make_zip("test.unzip.empty.zip", {{"directory/", ""}});
    EXPECT(Unzip::list(zip).empty());

    const PathName root("test.unzip.empty");
    const Unzip unzip(root);
    unzip.rm_cache_root();

    const auto dir = unzip.to_cached_path(zip);
    EXPECT(dir.isDir());

    unzip.rm_cache_root();
    zip.unlink();
}


CASE("to_path, list: not a zip archive is an error") {
    const PathName not_zip("test.unzip.not-a-zip.zip");
    write_file(not_zip, "not a zip archive");

    const PathName missing("test.unzip.missing.zip");
    ASSERT(!missing.exists());

    const PathName dir("test.unzip.not-a-zip");

    for (const auto& zip : {not_zip, missing}) {
        EXPECT_THROWS_AS(Unzip::list(zip), CantOpenFile);
        EXPECT_THROWS_AS(Unzip::to_path(zip, dir), CantOpenFile);
        EXPECT_NOT(dir.exists());
    }

    not_zip.unlink();
}


CASE("to_path, to_cached_path: entries outside of the extraction are an error (zip slip)") {
    const PathName dir("test.unzip.slip/extraction");
    const Unzip unzip(dir);

    for (const std::string& outside : {"../outside", "b/../../outside", "/tmp/eckit_test_cache_unzip_outside"}) {
        const auto zip = make_zip("test.unzip.slip.zip", {{"a", "inside"}, {outside, "outside"}});
        DiskCache::remove("test.unzip.slip");

        EXPECT_THROWS_AS(Unzip::to_path(zip, dir), ReadError);
        EXPECT_THROWS_AS(unzip.to_cached_path(zip), ReadError);

        // nothing written, outside or in the cache
        EXPECT_NOT(PathName("test.unzip.slip/outside").exists());
        EXPECT_NOT(PathName("/tmp/eckit_test_cache_unzip_outside").exists());
        EXPECT(files_without_locks(dir / "test.unzip.slip.zip.dir").empty());

        zip.unlink();
    }

    DiskCache::remove("test.unzip.slip");
}


CASE("to_cached_path: all files, extracted once") {
    const auto zip = make_zip("test.unzip.cached.zip", ENTRIES);

    const PathName root("test.unzip.cache");
    const Unzip unzip(root);
    unzip.rm_cache_root();

    const auto dir = unzip.to_cached_path(zip);
    EXPECT(dir.isDir());
    EXPECT(dir.dirName() == root / "test.unzip.cached.zip.dir");
    EXPECT(read_file(dir / "b/c") == "d\n");

    // found in the cache, not extracted again
    EXPECT(unzip.to_cached_path(zip) == dir);

    unzip.rm_cache_root();
    zip.unlink();
}


CASE("to_cached_path: a changed archive (at the same path) is extracted again") {
    const PathName root("test.unzip.cache");
    const Unzip unzip(root);
    unzip.rm_cache_root();

    const auto before = unzip.to_cached_path(make_zip("test.unzip.changed.zip", {{"file", "before"}}), "file");
    const auto after  = unzip.to_cached_path(make_zip("test.unzip.changed.zip", {{"file", "after, longer"}}), "file");

    EXPECT(before != after);
    EXPECT(read_file(after) == "after, longer");

    unzip.rm_cache_root();
    PathName("test.unzip.changed.zip").unlink();
}


CASE("to_cached_path: an entry, extracted once") {
    const auto zip = make_zip("test.unzip.cached.zip", ENTRIES);

    const PathName root("test.unzip.cache");
    const Unzip unzip(root);
    unzip.rm_cache_root();

    const auto file = unzip.to_cached_path(zip, "b/c");
    EXPECT_NOT(file.isDir());
    EXPECT(read_file(file) == "d\n");

    // read-only, as cache files are never modified
    struct stat st{};
    EXPECT(::stat(file.localPath(), &st) == 0 && (st.st_mode & 0777) == 0444);

    // each entry (and all files) is cached separately
    EXPECT(unzip.to_cached_path(zip, "b/c") == file);
    EXPECT(unzip.to_cached_path(zip, "a") != file);
    EXPECT(unzip.to_cached_path(zip) != file);

    // named with a prefix and suffix
    const std::string name = unzip.to_cached_path(zip, "a", "prefix", ".suffix").baseName();
    EXPECT(StringTools::startsWith(name, "prefix-") && StringTools::endsWith(name, ".suffix"));

    unzip.rm_cache_root();
    zip.unlink();
}


CASE("to_cached_path: a failed extraction is not cached") {
    const auto zip = make_zip("test.unzip.cached.zip", ENTRIES);

    const PathName root("test.unzip.cache");
    const Unzip unzip(root);
    unzip.rm_cache_root();

    EXPECT_THROWS_AS(unzip.to_cached_path(zip, "no/such/entry"), ReadError);
    EXPECT(files_without_locks(root / "test.unzip.cached.zip.dir").empty());

    unzip.rm_cache_root();
    zip.unlink();
}


CASE("to_cached_path: all files, by concurrent processes (extracted once)") {
    const auto zip = make_zip("test.unzip.processes.zip", ENTRIES);

    const PathName root("test.unzip.processes");
    const Unzip unzip(root);
    unzip.rm_cache_root();

    // start barrier: children block until the parent closes the pipe
    int barrier[2];
    ASSERT(::pipe(barrier) == 0);

    constexpr int N = 6;
    std::vector<pid_t> children;
    for (int i = 0; i < N; ++i) {
        auto pid = ::fork();
        ASSERT(pid >= 0);

        if (pid == 0) {
            ::close(barrier[1]);
            char c = 0;
            while (::read(barrier[0], &c, 1) > 0) {}

            int code = 1;
            try {
                // complete, once found
                const auto dir = unzip.to_cached_path(zip);
                code           = (dir / "a").exists() && read_file(dir / "b/c") == "d\n" ? 0 : 1;
            }
            catch (...) {
                code = 1;
            }
            ::_exit(code);
        }

        children.push_back(pid);
    }

    ::close(barrier[0]);
    ::close(barrier[1]);

    for (auto pid : children) {
        int status = 0;
        ASSERT(::waitpid(pid, &status, 0) == pid);
        EXPECT(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    }

    // one extraction, nothing partial left
    const auto cached = unzip.to_cached_path(zip).dirName();

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    cached.children(files, dirs);
    EXPECT(dirs.size() == 1);
    EXPECT(files_without_locks(cached).empty());

    unzip.rm_cache_root();
    zip.unlink();
}


CASE("an archive made by the zip tool (--unzip test.zip)") {
    const PathName zip(Resource<std::string>("--unzip", ""));
    if (zip.asString().empty()) {
        Log::info() << "skipped (no --unzip)" << std::endl;
        return;
    }

    // test.zip: a (empty), b/ and b/c ("d\n")
    EXPECT(Unzip::list(zip, false) == std::vector<std::string>({"a", "b/", "b/c"}));

    const PathName file("test.unzip.tool");
    Unzip::to_path(zip, file, "b/c");
    EXPECT(read_file(file) == "d\n");

    file.unlink();
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
