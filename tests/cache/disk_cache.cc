// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/PathLock.h"
#include "eckit/cache/mutex.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


// unique per test run (and process), so cache files are always new
PathName unique_path(const std::string& name) {
    static const auto dir = PathName::unique(PathName("eckit_cache/disk_cache/run"));
    return dir / name;
}


std::string read(const PathName& path) {
    std::ifstream in(path.localPath(), std::ios::binary);
    return {std::istreambuf_iterator<char>(in), {}};
}


void write(const PathName& path, const std::string& contents) {
    std::ofstream(path.localPath(), std::ios::binary) << contents;
}


// files (in the cache file directory) besides the cache file and its lock file
size_t count_other_files(const PathName& path) {
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    path.dirName().children(files, dirs);

    const auto base = path.baseName().asString();

    size_t count = dirs.size();
    for (const auto& f : files) {
        const auto name = f.baseName().asString();
        if (name != base && name != base + ".lock") {
            ++count;
        }
    }
    return count;
}


CASE("DiskCache::create: created once") {
    const auto path = unique_path("once");

    size_t written = 0;
    auto writer    = [&written, &path](const PathName& tmp) {
        // next to the cached file (same directory, so the same filesystem)
        EXPECT(tmp.dirName() == path.dirName());
        EXPECT(tmp != path);

        ++written;
        write(tmp, "contents");
    };

    EXPECT(DiskCache::create(path, writer));
    EXPECT(read(path) == "contents");

    EXPECT_NOT(DiskCache::create(path, writer));
    EXPECT(written == 1);

    EXPECT(count_other_files(path) == 0);
    DiskCache::remove(path.dirName());
}


CASE("DiskCache::create: an invalid file is recreated") {
    const auto path = unique_path("invalid");
    path.dirName().mkdir();
    write(path, "garbage");

    auto valid  = [](const PathName& p) { return read(p) == "valid"; };
    auto writer = [](const PathName& tmp) { write(tmp, "valid"); };

    EXPECT(DiskCache::create(path, writer, valid));
    EXPECT(read(path) == "valid");
    EXPECT_NOT(DiskCache::create(path, writer, valid));

    EXPECT(count_other_files(path) == 0);
    DiskCache::remove(path.dirName());
}


CASE("DiskCache::create: a failed creation leaves nothing behind") {
    const auto path = unique_path("failed");

    auto failing = [](const PathName& tmp) {
        write(tmp, "partial");
        throw std::runtime_error("failed");
    };

    EXPECT_THROWS_AS(DiskCache::create(path, failing), std::runtime_error);
    EXPECT_NOT(path.exists());
    EXPECT(count_other_files(path) == 0);

    // created on the next request
    EXPECT(DiskCache::create(path, [](const PathName& tmp) { write(tmp, "contents"); }));
    EXPECT(read(path) == "contents");

    DiskCache::remove(path.dirName());
}


CASE("DiskCache::get_or_create: found in any root, else created in the first writable root (only)") {
    const auto first  = unique_path("roots/first");
    const auto second = unique_path("roots/second");
    const DiskCache cache(Roots{std::vector<PathName>{first, second}});

    auto writer  = [](const PathName& tmp) { write(tmp, "created"); };
    bool created = false;

    SECTION("found in the second root (e.g. provisioned)") {
        second.mkdir();
        write(second / "found", "provisioned");

        EXPECT(cache.get_or_create("found", writer, {}, &created) == second / "found");
        EXPECT_NOT(created);
        EXPECT(read(second / "found") == "provisioned");
    }

    SECTION("created in the first root") {
        EXPECT(cache.get_or_create("created", writer, {}, &created) == first / "created");
        EXPECT(created);
    }

    SECTION("the first root failing (e.g. full) is the error, not written elsewhere") {
        auto failing = [](const PathName&) { throw FailedSystemCall("write (simulated)"); };
        EXPECT_THROWS_AS(cache.get_or_create("failed", failing), FailedSystemCall);
        EXPECT_NOT((second / "failed").exists());
    }

    DiskCache::remove(first);
    DiskCache::remove(second);
}


CASE("DiskCache::create: a directory") {
    const auto path = unique_path("directory");

    EXPECT(DiskCache::create(path, [](const PathName& tmp) {
        tmp.mkdir();
        write(tmp / "a", "a");
    }));

    EXPECT(path.isDir());
    EXPECT(read(path / "a") == "a");

    // removed, recursively
    DiskCache::remove(path);
    EXPECT_NOT(path.exists());

    DiskCache::remove(path.dirName());
}


CASE("PathLock: recursive within a thread") {
    const auto path = unique_path("lock");
    path.dirName().mkdir();

    PathLock a(path);
    PathLock b(path);
    EXPECT(a.lock_path().baseName() == path.baseName() + ".lock");

    {
        util::lock_guard<PathLock> lock_a(a);
        util::lock_guard<PathLock> lock_b(b);  // the same path, not a deadlock
        EXPECT(a.lock_path().exists());
    }

    // unlocked (as acquired again)
    util::lock_guard<PathLock> lock(a);

    DiskCache::remove(path.dirName());
}


CASE("PathLock: exclusive between threads, however the path is spelled") {
    const auto path = unique_path("exclusive");
    path.dirName().mkdir();

    // the same file, by another path (a symbolic link to its directory)
    const PathName link(path.dirName() + ".link");
    ASSERT(::symlink(path.dirName().realName().localPath(), link.localPath()) == 0);
    const auto spelled = link / path.baseName();
    EXPECT(PathLock(path).lock_path() == PathLock(spelled).lock_path());

    // threads hold the lock one at a time
    std::atomic<int> holding{0};
    std::atomic<bool> overlapped{false};

    auto hold = [&](const PathName& p) {
        for (int i = 0; i < 100; ++i) {
            PathLock locker(p);
            util::lock_guard<PathLock> lock(locker);

            if (++holding > 1) {
                overlapped = true;
            }
            std::this_thread::yield();
            --holding;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(hold, i % 2 == 0 ? path : spelled);
    }
    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_NOT(overlapped);

    link.unlink();
    DiskCache::remove(path.dirName());
}


CASE("DiskCache::create: an invalid directory is replaced") {
    const auto path = unique_path("invalid-directory");
    path.mkdir();
    write(path / "stale", "stale");

    auto valid  = [](const PathName& p) { return (p / "valid").exists(); };
    auto writer = [](const PathName& tmp) {
        tmp.mkdir();
        write(tmp / "valid", "valid");
    };

    EXPECT(DiskCache::create(path, writer, valid));
    EXPECT(read(path / "valid") == "valid");
    EXPECT_NOT((path / "stale").exists());

    // nothing else left (e.g. the invalid directory, moved aside)
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    path.dirName().children(files, dirs);
    EXPECT(dirs.size() == 1);

    DiskCache::remove(path.dirName());
}


CASE("DiskCache::create: created once, by concurrent processes") {
    const auto path = unique_path("processes");

    constexpr int N       = 6;
    constexpr int CREATED = 10;
    constexpr int FOUND   = 20;
    constexpr int FAILED  = 1;

    // start barrier: children block until the parent closes the pipe
    int barrier[2];
    ASSERT(::pipe(barrier) == 0);

    std::vector<pid_t> children;
    for (int i = 0; i < N; ++i) {
        auto pid = ::fork();
        ASSERT(pid >= 0);

        if (pid == 0) {
            ::close(barrier[1]);
            char c = 0;
            while (::read(barrier[0], &c, 1) > 0) {}

            int code = FAILED;
            try {
                const bool created = DiskCache::create(path, [](const PathName& tmp) {
                    write(tmp, "contents");
                    ::usleep(100000);  // so the other processes find the creation in progress
                });
                code               = read(path) == "contents" ? (created ? CREATED : FOUND) : FAILED;
            }
            catch (...) {
                code = FAILED;
            }
            ::_exit(code);
        }

        children.push_back(pid);
    }

    ::close(barrier[0]);
    ::close(barrier[1]);

    int created = 0;
    int found   = 0;
    for (auto pid : children) {
        int status = 0;
        ASSERT(::waitpid(pid, &status, 0) == pid);
        EXPECT(WIFEXITED(status));

        auto code = WEXITSTATUS(status);
        EXPECT(code == CREATED || code == FOUND);
        created += code == CREATED ? 1 : 0;
        found += code == FOUND ? 1 : 0;
    }

    EXPECT(created == 1);
    EXPECT(found == N - 1);

    EXPECT(count_other_files(path) == 0);
    DiskCache::remove(path.dirName());
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
