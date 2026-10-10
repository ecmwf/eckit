// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/cache/Version.h"
#include "eckit/geo/search/FileIO.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/search/LoadedFile.h"
#include "eckit/geo/search/MappedFile.h"
#include "eckit/geo/search/Memory.h"
#include "eckit/geo/search/SharedMemory.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using search::FileIO;
using search::KDTree;
using search::KDTreeFactory;
using search::MappedFile;
using search::SharedMemory;
using search::TempSharedMemory;


// one name per loader (see the aliases in KDTreeFactory test)
const std::vector<std::string> LOADERS{"memory", "mmap", "file-io", "shmem", "tmp-shmem"};


// unique per test run (and process), so cached k-d trees are always new
std::string unique_uid(const std::string& name) {
    static const auto salt =
        std::to_string(::getpid()) + "-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    return "test-" + name + "-" + salt;
}


std::vector<PointXYZ> random_points(size_t n, unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(-1., 1.);

    std::vector<PointXYZ> points;
    points.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        points.emplace_back(dist(gen), dist(gen), dist(gen));
    }
    return points;
}


std::vector<KDTree::Value> values(const std::vector<PointXYZ>& points) {
    std::vector<KDTree::Value> v;
    v.reserve(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        v.emplace_back(points[i], i);
    }
    return v;
}


// if every point finds itself
bool consistent(KDTree& kdtree, const std::vector<PointXYZ>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        if (auto n = kdtree.nearest_neighbour(points[i]); n.index != i || n.distance != 0.) {
            return false;
        }
    }
    return true;
}


bool consistent(const Search& search, const std::vector<PointXYZ>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        if (auto n = search.search_nn(points[i]); n.index != i || n.distance != 0.) {
            return false;
        }
    }
    return true;
}


/// Build k-d tree if not ready, @return if built
bool load_or_build(KDTree& kdtree, const std::vector<PointXYZ>& points) {
    std::lock_guard<KDTree> lock(kdtree);

    if (kdtree.ready()) {
        return false;
    }

    auto v = values(points);
    kdtree.build(v);
    kdtree.commit();
    return true;
}


// temporary files (in the k-d tree directory) besides the k-d tree and its lock file
size_t count_temporary_files(const PathName& path) {
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    path.dirName().children(files, dirs);

    const auto base = path.baseName().asString();

    size_t count = 0;
    for (const auto& f : files) {
        const auto name = f.baseName().asString();
        if (name != base && name != base + ".lock" && name.rfind(base, 0) == 0) {
            ++count;
        }
    }
    return count;
}


// cache file, its lock file and its shared memory segment
void remove(const PathName& path) {
    SharedMemory::unload(path);
    for (const auto& p : {path, PathName{path + ".lock"}}) {
        if (p.exists()) {
            p.unlink(false);
        }
    }
}


CASE("KDTreeFactory") {
    for (const auto& name : LOADERS) {
        EXPECT(KDTreeFactory::has_type(name));
    }

    EXPECT_NOT(KDTreeFactory::has_type("unknown"));
    EXPECT_NOT(KDTreeFactory::has_type("mapped-cache-file"));  // now mapped-memory
    EXPECT_THROWS_AS(std::unique_ptr<KDTree>(KDTreeFactory::build("unknown", "uid", 1)), exception::SearchError);
    EXPECT_THROWS_AS(KDTreeFactory::estimate("unknown", 1), exception::SearchError);

    // the default loader
    EXPECT_EQUAL(LibEcKitGeo::searchLoader(), "mapped-memory");

    SECTION("aliases") {
        auto is = [](const std::string& name, auto* type) {
            std::unique_ptr<KDTree> kdtree(KDTreeFactory::build(name, unique_uid("alias-" + name), 1));
            return dynamic_cast<decltype(type)>(kdtree.get()) != nullptr;
        };

        EXPECT(is("memory", static_cast<search::Memory*>(nullptr)));
        EXPECT(is("file-io", static_cast<FileIO*>(nullptr)));

        for (const auto& name : {"mmap", "mapped-memory"}) {
            EXPECT(is(name, static_cast<MappedFile*>(nullptr)));
            EXPECT_NOT(is(name, static_cast<search::LoadedFile*>(nullptr)));
        }

        for (const auto& name : {"shmem", "shared-memory"}) {
            EXPECT(is(name, static_cast<SharedMemory*>(nullptr)));
            EXPECT_NOT(is(name, static_cast<TempSharedMemory*>(nullptr)));
        }

        for (const auto& name : {"tmp-shmem", "tmp-shared-memory"}) {
            EXPECT(is(name, static_cast<TempSharedMemory*>(nullptr)));
        }
    }

    // empty trees are not supported
    for (const auto& name : LOADERS) {
        EXPECT_THROWS_AS(std::unique_ptr<KDTree>(KDTreeFactory::build(name, unique_uid("empty"), 0)),
                         exception::SearchError);
    }
}


CASE("KDTree: build and query") {
    const auto points = random_points(500, 1);

    for (const auto& name : LOADERS) {
        for (bool fast : {true, false}) {
            SECTION(name + (fast ? " (build)" : " (insert)")) {
                const auto uid = unique_uid("build-" + name + (fast ? "-build" : "-insert"));

                std::unique_ptr<KDTree> kdtree(KDTreeFactory::build(name, uid, points.size()));
                EXPECT_EQUAL(kdtree->uid(), uid);
                EXPECT_EQUAL(kdtree->size(), points.size());

                {
                    std::lock_guard<KDTree> lock(*kdtree);
                    EXPECT_NOT(kdtree->ready());

                    if (fast) {
                        auto v = values(points);
                        kdtree->build(v);
                    }
                    else {
                        for (const auto& v : values(points)) {
                            kdtree->insert(v);
                        }
                    }

                    kdtree->commit();
                }

                EXPECT(consistent(*kdtree, points));

                auto knn = kdtree->k_nearest_neighbours(points[0], 3);
                EXPECT_EQUAL(knn.size(), 3);
                EXPECT_EQUAL(knn.front().index, 0);

                auto sphere = kdtree->find_in_sphere(points[0], 10.);  // all points
                EXPECT_EQUAL(sphere.size(), points.size());

                if (auto* file = dynamic_cast<MappedFile*>(kdtree.get()); file != nullptr) {
                    EXPECT(file->path().exists());
                    EXPECT_EQUAL(count_temporary_files(file->path()), 0);
                    remove(file->path());
                }
            }
        }
    }
}


CASE("KDTreeFactory: estimate is the footprint, once built or loaded") {
    const auto points = random_points(1000, 11);

    for (const auto& name : LOADERS) {
        SECTION(name) {
            const auto estimate = KDTreeFactory::estimate(name, points.size());
            EXPECT(estimate);

            for (bool build : {true, false}) {
                std::unique_ptr<KDTree> kdtree(
                    KDTreeFactory::build(name, unique_uid("estimate-" + name), points.size()));
                EXPECT_EQUAL(load_or_build(*kdtree, points), build);

                // shared memory (not available, e.g. system limits) falls back to the cache file
                if (auto* shm = dynamic_cast<SharedMemory*>(kdtree.get()); shm != nullptr && !shm->loaded()) {
                    Log::warning() << "shared memory not available, skipping '" << name << "'" << std::endl;
                    remove(shm->path());
                    break;
                }

                EXPECT_EQUAL(kdtree->footprint(), estimate);

                // in-process k-d trees are built every time
                auto* file = dynamic_cast<MappedFile*>(kdtree.get());
                if (file == nullptr) {
                    break;
                }

                if (!build) {
                    remove(file->path());
                }
            }
        }
    }
}


CASE("KDTree: capacity") {
    const auto points = random_points(10, 2);

    for (const auto& name : LOADERS) {
        SECTION(name) {
            std::unique_ptr<KDTree> kdtree(
                KDTreeFactory::build(name, unique_uid("capacity-" + name), points.size() - 1));

            {
                std::lock_guard<KDTree> lock(*kdtree);

                auto v = values(points);
                EXPECT_THROWS(kdtree->build(v));
            }

            if (auto* file = dynamic_cast<MappedFile*>(kdtree.get()); file != nullptr) {
                EXPECT_NOT(file->path().exists());
                remove(file->path());
            }
        }
    }
}


CASE("MappedFile: location") {
    // ECKIT_GEO_CACHE_PATH is set by the test environment
    const auto* env = ::getenv("ECKIT_GEO_CACHE_PATH");
    if (env != nullptr) {
        EXPECT_EQUAL(LibEcKitGeo::cacheDir(), PathName(env, true).asString());
    }

    const auto uid = unique_uid("location");
    MappedFile kdtree(uid, 1);

    const auto expected =
        PathName{LibEcKitGeo::cacheDir()} / "search" / std::to_string(cache::version::SEARCH) / (uid + ".kdtree");
    EXPECT_EQUAL(kdtree.path(), expected);
    EXPECT_NOT(kdtree.path().exists());

    // uid is part of the path
    for (const std::string& uid : {"", "../x", "a/b", "a.b", "a b"}) {
        EXPECT_THROWS_AS(MappedFile(uid, 1), exception::SearchError);
    }

    // root not writable
    EXPECT_THROWS_AS(MappedFile::kdtree_path(uid, PathName{"/dev/null/not-a-directory"}), exception::SearchError);
}


CASE("MappedFile: build, commit, load") {
    const auto points = random_points(1000, 3);
    const auto uid    = unique_uid("load");

    PathName path;

    {
        MappedFile kdtree(uid, points.size());
        path = kdtree.path();
        remove(path);

        EXPECT(load_or_build(kdtree, points));
        EXPECT(path.exists());
        EXPECT(consistent(kdtree, points));
    }

    {
        MappedFile kdtree(uid, points.size());
        EXPECT_NOT(load_or_build(kdtree, points));
        EXPECT(consistent(kdtree, points));
    }

    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("MappedFile: invalid file is rebuilt") {
    const auto points = random_points(100, 4);
    const auto uid    = unique_uid("invalid");

    MappedFile kdtree(uid, points.size());
    const auto path = kdtree.path();

    SECTION("garbage") {
        path.dirName().mkdir();
        std::ofstream(path.localPath()) << "garbage";
        EXPECT(path.exists());

        EXPECT(load_or_build(kdtree, points));
        EXPECT(consistent(kdtree, points));
    }

    SECTION("different size") {
        // same uid (path), different number of points
        const auto other = random_points(points.size() + 1, 5);
        {
            MappedFile kdtree_other(uid, other.size());
            EXPECT(load_or_build(kdtree_other, other));
        }

        EXPECT(load_or_build(kdtree, points));
        EXPECT(consistent(kdtree, points));
    }

    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("MappedFile: uncommitted build is discarded") {
    const auto points = random_points(100, 6);
    const auto uid    = unique_uid("uncommitted");

    PathName path;
    {
        MappedFile kdtree(uid, points.size());
        path = kdtree.path();

        std::lock_guard<KDTree> lock(kdtree);
        EXPECT_NOT(kdtree.ready());

        auto v = values(points);
        kdtree.build(v);

        EXPECT_EQUAL(count_temporary_files(path), 1);
    }

    EXPECT_NOT(path.exists());
    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("MappedFile: multi-threaded") {
    const auto points = random_points(50000, 7);
    const auto uid    = unique_uid("threads");

    constexpr int N = 8;

    std::atomic<int> built{0};
    std::atomic<int> failed{0};
    std::atomic<bool> go{false};

    std::vector<std::thread> threads;
    for (int i = 0; i < N; ++i) {
        threads.emplace_back([&]() {
            while (!go) {
                std::this_thread::yield();
            }

            try {
                MappedFile kdtree(uid, points.size());
                if (load_or_build(kdtree, points)) {
                    ++built;
                }
                if (!consistent(kdtree, points)) {
                    ++failed;
                }
            }
            catch (...) {
                ++failed;
            }
        });
    }

    go = true;
    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQUAL(failed.load(), 0);
    EXPECT_EQUAL(built.load(), 1);

    MappedFile kdtree(uid, points.size());
    EXPECT_EQUAL(count_temporary_files(kdtree.path()), 0);
    remove(kdtree.path());
}


CASE("MappedFile: multi-process") {
    const auto points = random_points(50000, 8);
    const auto uid    = unique_uid("processes");

    constexpr int N      = 6;
    constexpr int BUILT  = 10;
    constexpr int LOADED = 20;
    constexpr int FAILED = 1;

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
                MappedFile kdtree(uid, points.size());
                const bool built = load_or_build(kdtree, points);
                code             = consistent(kdtree, points) ? (built ? BUILT : LOADED) : FAILED;
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

    int built  = 0;
    int loaded = 0;
    for (auto pid : children) {
        int status = 0;
        ASSERT(::waitpid(pid, &status, 0) == pid);
        EXPECT(WIFEXITED(status));

        auto code = WEXITSTATUS(status);
        EXPECT(code == BUILT || code == LOADED);
        built += code == BUILT ? 1 : 0;
        loaded += code == LOADED ? 1 : 0;
    }

    EXPECT_EQUAL(built, 1);
    EXPECT_EQUAL(loaded, N - 1);

    MappedFile kdtree(uid, points.size());
    EXPECT_NOT(load_or_build(kdtree, points));
    EXPECT(consistent(kdtree, points));

    EXPECT_EQUAL(count_temporary_files(kdtree.path()), 0);
    remove(kdtree.path());
}


CASE("FileIO: loaded from the cache file, into memory") {
    const auto points = random_points(1000, 12);
    const auto uid    = unique_uid("file-io");

    // built by another loader of the same cache file
    PathName path;
    {
        MappedFile kdtree(uid, points.size());
        EXPECT(load_or_build(kdtree, points));
        path = kdtree.path();
    }

    FileIO kdtree(uid, points.size());
    EXPECT_EQUAL(kdtree.path(), path);
    EXPECT_NOT(kdtree.loaded());

    EXPECT_NOT(load_or_build(kdtree, points));
    EXPECT(kdtree.loaded());
    EXPECT(consistent(kdtree, points));

    // in memory, the cache file is no longer mapped
    EXPECT_EQUAL(kdtree.footprint(), cache::MemoryUsage(static_cast<size_t>(path.size()), 0));

    remove(path);
}


CASE("SharedMemory: shared between loaders, until unloaded") {
    const auto points = random_points(1000, 13);
    const auto uid    = unique_uid("shmem");

    SharedMemory a(uid, points.size());
    EXPECT(load_or_build(a, points));

    if (!a.loaded()) {
        Log::warning() << "shared memory not available, skipping" << std::endl;
        remove(a.path());
        return;
    }

    EXPECT(SharedMemory::is_loaded(a.path()));

    // attached, not loaded again
    SharedMemory b(uid, points.size());
    EXPECT_NOT(load_or_build(b, points));
    EXPECT(b.loaded());
    EXPECT(consistent(b, points));

    // removed, though usable while attached
    SharedMemory::unload(a.path());
    EXPECT_NOT(SharedMemory::is_loaded(a.path()));
    EXPECT(consistent(a, points));
    EXPECT(consistent(b, points));

    remove(a.path());
}


CASE("TempSharedMemory: removed at exit") {
    const auto points = random_points(1000, 14);

    constexpr int LOADED        = 0;
    constexpr int NOT_AVAILABLE = 1;
    constexpr int FAILED        = 2;

    // load in a child process, which exits normally (std::exit, so static objects are destructed)
    auto load_and_exit = [&points](const std::string& loader, const std::string& uid) {
        std::cout.flush();
        std::fflush(nullptr);  // not to repeat buffered output

        auto pid = ::fork();
        ASSERT(pid >= 0);

        if (pid == 0) {
            int code = FAILED;
            try {
                std::unique_ptr<KDTree> kdtree(KDTreeFactory::build(loader, uid, points.size()));
                load_or_build(*kdtree, points);
                code = dynamic_cast<SharedMemory&>(*kdtree).loaded() ? LOADED : NOT_AVAILABLE;
            }
            catch (...) {
                code = FAILED;
            }
            std::exit(code);
        }

        int status = 0;
        ASSERT(::waitpid(pid, &status, 0) == pid);
        ASSERT(WIFEXITED(status));
        return WEXITSTATUS(status);
    };

    for (const auto& loader_removed :
         std::vector<std::pair<std::string, bool>>{{"tmp-shmem", true}, {"shmem", false}}) {
        const auto& loader = loader_removed.first;
        const bool removed = loader_removed.second;

        SECTION(loader) {
            const auto uid  = unique_uid("exit-" + loader);
            const auto code = load_and_exit(loader, uid);
            EXPECT(code != FAILED);

            const auto path = MappedFile::kdtree_path(uid, PathName{LibEcKitGeo::cacheDir()});
            if (code == LOADED) {
                EXPECT_EQUAL(SharedMemory::is_loaded(path), !removed);
            }
            else {
                Log::warning() << "shared memory not available, skipping '" << loader << "'" << std::endl;
            }

            remove(path);
        }
    }
}


CASE("Search: multi-threaded") {
    // unique points (uid), so the cached k-d tree is new
    auto points = random_points(5000, 9);
    points.emplace_back(10., static_cast<double>(::getpid()),
                        static_cast<double>(std::chrono::steady_clock::now().time_since_epoch().count() % 1000000));

    const spec::Custom spec{{{"search-loader", "mmap"}}};

    constexpr int N = 8;
    std::atomic<int> failed{0};

    SECTION("construct concurrently") {
        std::vector<std::thread> threads;
        for (int i = 0; i < N; ++i) {
            threads.emplace_back([&]() {
                try {
                    const Search search(points, spec);
                    if (!consistent(search, points)) {
                        ++failed;
                    }
                }
                catch (...) {
                    ++failed;
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        EXPECT_EQUAL(failed.load(), 0);
    }

    SECTION("query concurrently") {
        const Search search(points, spec);

        std::vector<std::thread> threads;
        for (int i = 0; i < N; ++i) {
            threads.emplace_back([&]() {
                if (!consistent(search, points)) {
                    ++failed;
                }
                for (const auto& p : points) {
                    if (search.search_knn(p, 4).size() != 4) {
                        ++failed;
                    }
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        EXPECT_EQUAL(failed.load(), 0);
    }

    const Search search(points, spec);
    const auto& path = dynamic_cast<const MappedFile&>(search.kdtree()).path();
    EXPECT(path.exists());
    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("Search: caching") {
    const auto points = random_points(10, 10);

    // caching disabled: memory, unless explicitly requested
    EXPECT(dynamic_cast<const search::Memory*>(&Search(points, spec::Custom{{{"caching", false}}}).kdtree()) !=
           nullptr);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
