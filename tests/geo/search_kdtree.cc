// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "eckit/cache/FileCache.h"
#include "eckit/cache/Loader.h"
#include "eckit/cache/loader/File.h"
#include "eckit/cache/loader/MappedAnonymousMemory.h"
#include "eckit/cache/loader/MappedFile.h"
#include "eckit/cache/loader/SharedMemory.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/search/KDTreeCache.h"
#include "eckit/geo/search/KDTreeMemory.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using search::KDTree;
using search::KDTreeCache;
using search::KDTreeFactory;


// one name per loader (see the cache loaders aliases, in eckit::cache tests)
const std::vector<std::string> LOADERS{"memory", "mmap", "mapped-anonymous-memory", "file", "shmem", "tmp-shmem"};


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


KDTree::Fill fill(const std::vector<PointXYZ>& points) {
    return [&points](std::vector<KDTree::Value>& values) {
        for (size_t i = 0; i < points.size(); ++i) {
            values.emplace_back(points[i], i);
        }
    };
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


// cache file of a k-d tree, in the cache directory for writing (see LibEcKitGeo::cacheDir)
PathName cache_file(const std::string& uid) {
    return PathName{LibEcKitGeo::cacheDir()} / geo::cache::SearchCache::file_cache().relative(uid);
}


// cache file, its lock file and its shared memory segment
void remove(const PathName& path) {
    eckit::cache::loader::SharedMemory::unload(path);
    for (const auto& p : {path, PathName{path + ".lock"}}) {
        if (p.exists()) {
            p.unlink(false);
        }
    }
}


CASE("KDTreeFactory") {
    for (const auto& name : LOADERS) {
        EXPECT(KDTreeFactory::has(name));
        EXPECT(KDTreeFactory::estimate(name, 1));
    }

    // unknown, or not loading k-d tree cache files (stream: not the k-d tree layout)
    for (const auto& name : {"unknown", "stream"}) {
        EXPECT_NOT(KDTreeFactory::has(name));
        EXPECT_THROWS_AS(KDTreeFactory::estimate(name, 1), exception::SearchError);
        EXPECT_THROWS_AS(std::unique_ptr<KDTree>(KDTreeFactory::build(name, "uid", 1)), exception::SearchError);
    }

    // a cache file is always loaded from a file
    EXPECT_THROWS_AS(KDTreeCache("memory", "uid", 1), exception::SearchError);

    // the default loader
    EXPECT(LibEcKitGeo::searchCacheLoader() == "mapped-file");

    SECTION("types") {
        const auto points = random_points(10, 15);

        std::unique_ptr<KDTree> memory(KDTreeFactory::build("memory", unique_uid("type-memory"), points.size()));
        EXPECT(dynamic_cast<search::KDTreeMemory*>(memory.get()) != nullptr);

        // any other loader is of the cache file
        auto is = [&points](const std::string& name, auto* type) {
            std::unique_ptr<KDTree> kdtree(KDTreeFactory::build(name, unique_uid("type-" + name), points.size()));
            auto& cached = dynamic_cast<KDTreeCache&>(*kdtree);
            cached.build(fill(points));

            const bool is_type = dynamic_cast<decltype(type)>(&cached.loader()) != nullptr;
            remove(cached.path());
            return is_type;
        };

        EXPECT(is("file", static_cast<const eckit::cache::loader::File*>(nullptr)));
        EXPECT(is("mmap", static_cast<const eckit::cache::loader::MappedFile*>(nullptr)));
        EXPECT(is("mapped-anonymous-memory", static_cast<const eckit::cache::loader::MappedAnonymousMemory*>(nullptr)));
        EXPECT(is("shmem", static_cast<const eckit::cache::loader::SharedMemory*>(nullptr)));
        EXPECT(is("tmp-shmem", static_cast<const eckit::cache::loader::TempSharedMemory*>(nullptr)));
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
        for (bool balanced : {true, false}) {
            SECTION(name + (balanced ? " (balanced)" : " (insertion)")) {
                const auto uid = unique_uid("build-" + name + (balanced ? "-balanced" : "-insertion"));

                std::unique_ptr<KDTree> kdtree(KDTreeFactory::build(name, uid, points.size()));
                EXPECT(kdtree->size() == points.size());

                EXPECT(kdtree->build(fill(points), balanced));
                EXPECT_NOT(kdtree->build(fill(points), balanced));  // built already

                EXPECT(consistent(*kdtree, points));

                auto knn = kdtree->k_nearest_neighbours(points[0], 3);
                EXPECT(knn.size() == 3);
                EXPECT(knn.front().index == 0);

                auto sphere = kdtree->find_in_sphere(points[0], 10.);  // all points
                EXPECT(sphere.size() == points.size());

                if (auto* cached = dynamic_cast<KDTreeCache*>(kdtree.get()); cached != nullptr) {
                    EXPECT(cached->path().exists());
                    EXPECT(count_temporary_files(cached->path()) == 0);
                    remove(cached->path());
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
                EXPECT(kdtree->build(fill(points)) == build);

                auto* cached = dynamic_cast<KDTreeCache*>(kdtree.get());

                EXPECT(kdtree->footprint() == estimate);

                // in-process k-d trees are built every time
                if (cached == nullptr) {
                    break;
                }

                if (!build) {
                    remove(cached->path());
                }
            }
        }
    }
}


CASE("KDTree: the values must be as many as the points") {
    const auto points = random_points(10, 2);

    for (const auto& name : LOADERS) {
        SECTION(name) {
            for (size_t size : {points.size() - 1, points.size() + 1}) {
                const auto uid = unique_uid("capacity-" + name);
                std::unique_ptr<KDTree> kdtree(KDTreeFactory::build(name, uid, size));
                EXPECT_THROWS_AS(kdtree->build(fill(points)), exception::SearchError);

                if (auto* cached = dynamic_cast<KDTreeCache*>(kdtree.get()); cached != nullptr) {
                    const auto path = cache_file(uid);
                    EXPECT_NOT(path.exists());
                    EXPECT(count_temporary_files(path) == 0);
                    remove(path);
                }
            }
        }
    }
}


CASE("KDTreeCache: location") {
    // ECKIT_GEO_CACHE_PATH is set by the test environment
    const auto* env = ::getenv("ECKIT_GEO_CACHE_PATH");
    if (env != nullptr) {
        EXPECT(LibEcKitGeo::cacheDir() == PathName(env, true).asString());
    }

    const auto uid = unique_uid("location");
    KDTreeCache kdtree("mmap", uid, 1);

    EXPECT(geo::cache::SearchCache::file_cache().relative(uid) ==
           PathName{"search"} / std::to_string(geo::cache::SearchCache::VERSION) / (uid + ".kdtree"));

    // not built yet
    EXPECT_THROWS_AS(kdtree.path(), exception::SearchError);
    EXPECT_THROWS_AS(kdtree.loader(), exception::SearchError);
    EXPECT(kdtree.footprint() == KDTree::MemoryUsage());

    // uid is part of the path
    for (const std::string& uid : {"", "../x", "a/b", "a.b", "a b"}) {
        EXPECT_THROWS_AS(KDTreeCache("mmap", uid, 1), exception::SearchError);
    }

    // built in the cache directory for writing
    const std::vector<PointXYZ> points{{1., 0., 0.}};
    EXPECT(kdtree.build(fill(points)));
    EXPECT(kdtree.path() == cache_file(uid));
    remove(kdtree.path());
}


CASE("KDTreeCache: built, then loaded") {
    const auto points = random_points(1000, 3);
    const auto uid    = unique_uid("load");

    PathName path;

    {
        KDTreeCache kdtree("mmap", uid, points.size());

        EXPECT(kdtree.build(fill(points)));
        path = kdtree.path();
        EXPECT(path.exists());
        EXPECT(KDTreeCache::valid(path, points.size()));
        EXPECT(consistent(kdtree, points));
    }

    // by any loader
    for (const auto& name : {"mmap", "file", "shmem"}) {
        KDTreeCache kdtree(name, uid, points.size());
        EXPECT_NOT(kdtree.build(fill(points)));
        EXPECT(consistent(kdtree, points));
    }

    EXPECT(count_temporary_files(path) == 0);
    remove(path);
}


CASE("KDTreeCache: an invalid cache file is rebuilt") {
    const auto points = random_points(100, 4);
    const auto uid    = unique_uid("invalid");

    KDTreeCache kdtree("mmap", uid, points.size());
    const auto path = cache_file(uid);

    SECTION("garbage") {
        path.dirName().mkdir();
        std::ofstream(path.localPath()) << "garbage";
        EXPECT(path.exists());
        EXPECT_NOT(KDTreeCache::valid(path, points.size()));

        EXPECT(kdtree.build(fill(points)));
        EXPECT(consistent(kdtree, points));
    }

    SECTION("different size") {
        // same uid (path), different number of points
        const auto other = random_points(points.size() + 1, 5);
        {
            KDTreeCache kdtree_other("mmap", uid, other.size());
            EXPECT(kdtree_other.build(fill(other)));
        }

        EXPECT(kdtree.build(fill(points)));
        EXPECT(consistent(kdtree, points));
    }

    EXPECT(count_temporary_files(path) == 0);
    remove(path);
}


CASE("KDTreeCache: a failed build is discarded") {
    const auto points = random_points(100, 6);
    const auto uid    = unique_uid("failed");

    KDTreeCache kdtree("mmap", uid, points.size());
    const auto path = cache_file(uid);

    auto failing = [](std::vector<KDTree::Value>&) { throw std::runtime_error("failed"); };
    EXPECT_THROWS_AS(kdtree.build(failing), std::runtime_error);

    EXPECT_NOT(path.exists());
    EXPECT(count_temporary_files(path) == 0);

    // built on the next request
    EXPECT(kdtree.build(fill(points)));
    EXPECT(consistent(kdtree, points));

    remove(path);
}


CASE("KDTreeCache: built once, by concurrent processes") {
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
                KDTreeCache kdtree("mmap", uid, points.size());
                const bool built = kdtree.build(fill(points));
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

    EXPECT(built == 1);
    EXPECT(loaded == N - 1);

    KDTreeCache kdtree("mmap", uid, points.size());
    EXPECT_NOT(kdtree.build(fill(points)));
    EXPECT(consistent(kdtree, points));

    EXPECT(count_temporary_files(kdtree.path()) == 0);
    remove(kdtree.path());
}


CASE("Search: the loader is the library's") {
    EXPECT(LibEcKitGeo::caching());  // as tested
    EXPECT(LibEcKitGeo::searchCacheLoader() == "mapped-file");

    const auto points = random_points(10, 10);
    const Search search(points);

    const auto* cached = dynamic_cast<const KDTreeCache*>(&search.kdtree());
    EXPECT(cached != nullptr);
    EXPECT(dynamic_cast<const eckit::cache::loader::MappedFile*>(&cached->loader()) != nullptr);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
