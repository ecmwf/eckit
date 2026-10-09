// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/search/Tree.h"
#include "eckit/geo/search/TreeMappedFile.h"
#include "eckit/geo/search/TreeMemory.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using search::Tree;
using search::TreeFactory;
using search::TreeMappedCacheFile;
using search::TreeMappedFile;


const std::vector<std::string> TREES{"memory", "mapped-anonymous-memory", "mapped-temporary-file", "mapped-cache-file"};


// unique per test run (and process), so cached trees are always new
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


std::vector<Tree::Value> values(const std::vector<PointXYZ>& points) {
    std::vector<Tree::Value> v;
    v.reserve(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        v.emplace_back(points[i], i);
    }
    return v;
}


// if every point finds itself
bool consistent(Tree& tree, const std::vector<PointXYZ>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        if (auto n = tree.nearest_neighbour(points[i]); n.index != i || n.distance != 0.) {
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


/// Build tree if not ready, @return if built
bool load_or_build(Tree& tree, const std::vector<PointXYZ>& points) {
    std::lock_guard<Tree> lock(tree);

    if (tree.ready()) {
        return false;
    }

    auto v = values(points);
    tree.build(v);
    tree.commit();
    return true;
}


// temporary files (in the tree directory) besides the tree and its lock file
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


void remove(const PathName& path) {
    for (const auto& p : {path, PathName{path + ".lock"}}) {
        if (p.exists()) {
            p.unlink(false);
        }
    }
}


CASE("TreeFactory") {
    for (const auto& name : TREES) {
        EXPECT(TreeFactory::has_type(name));
    }

    EXPECT_NOT(TreeFactory::has_type("unknown"));
    EXPECT(TreeFactory::has_type("shared-memory"));
    EXPECT_THROWS_AS(std::unique_ptr<Tree>(TreeFactory::build("unknown", "uid", 1)), exception::SearchError);

    // empty trees are not supported
    for (const auto& name : TREES) {
        EXPECT_THROWS_AS(std::unique_ptr<Tree>(TreeFactory::build(name, unique_uid("empty"), 0)),
                         exception::SearchError);
    }
}


CASE("Tree: build and query") {
    const auto points = random_points(500, 1);

    for (const auto& name : TREES) {
        for (bool fast : {true, false}) {
            SECTION(name + (fast ? " (build)" : " (insert)")) {
                const auto uid = unique_uid("build-" + name + (fast ? "-build" : "-insert"));

                std::unique_ptr<Tree> tree(TreeFactory::build(name, uid, points.size()));
                EXPECT_EQUAL(tree->uid(), uid);
                EXPECT_EQUAL(tree->size(), points.size());

                {
                    std::lock_guard<Tree> lock(*tree);
                    EXPECT_NOT(tree->ready());

                    if (fast) {
                        auto v = values(points);
                        tree->build(v);
                    }
                    else {
                        for (const auto& v : values(points)) {
                            tree->insert(v);
                        }
                    }

                    tree->commit();
                }

                EXPECT(consistent(*tree, points));

                auto knn = tree->k_nearest_neighbours(points[0], 3);
                EXPECT_EQUAL(knn.size(), 3);
                EXPECT_EQUAL(knn.front().index, 0);

                auto sphere = tree->find_in_sphere(points[0], 10.);  // all points
                EXPECT_EQUAL(sphere.size(), points.size());

                if (auto* file = dynamic_cast<TreeMappedFile*>(tree.get()); file != nullptr) {
                    EXPECT(file->path().exists());
                    EXPECT_EQUAL(count_temporary_files(file->path()), 0);
                    remove(file->path());
                }
            }
        }
    }
}


CASE("Tree: capacity") {
    const auto points = random_points(10, 2);

    for (const auto& name : TREES) {
        SECTION(name) {
            std::unique_ptr<Tree> tree(TreeFactory::build(name, unique_uid("capacity-" + name), points.size() - 1));

            {
                std::lock_guard<Tree> lock(*tree);

                auto v = values(points);
                EXPECT_THROWS(tree->build(v));
            }

            if (auto* file = dynamic_cast<TreeMappedFile*>(tree.get()); file != nullptr) {
                EXPECT_NOT(file->path().exists());
                remove(file->path());
            }
        }
    }
}


CASE("TreeMappedFile: location") {
    // ECKIT_GEO_CACHE_PATH is set by the test environment
    const auto* env = ::getenv("ECKIT_GEO_CACHE_PATH");
    if (env != nullptr) {
        EXPECT_EQUAL(LibEcKitGeo::cacheDir(), PathName(env, true).asString());
    }

    const auto roots = TreeMappedCacheFile::roots();
    EXPECT_EQUAL(roots.size(), 1);
    EXPECT_EQUAL(roots.front(), PathName{LibEcKitGeo::cacheDir()});

    const auto uid = unique_uid("location");
    TreeMappedCacheFile tree(uid, 1);

    const auto expected =
        PathName{LibEcKitGeo::cacheDir()} / "search" / std::to_string(TreeMappedFile::VERSION) / (uid + ".kdtree");
    EXPECT_EQUAL(tree.path(), expected);
    EXPECT_NOT(tree.path().exists());

    // uid is part of the path
    for (const std::string& uid : {"", "../x", "a/b", "a.b", "a b"}) {
        EXPECT_THROWS_AS(TreeMappedCacheFile(uid, 1), exception::SearchError);
    }

    // no writable root
    EXPECT_THROWS_AS(TreeMappedFile(uid, 1, {PathName{"/dev/null/not-a-directory"}}), exception::SearchError);
}


CASE("TreeMappedFile: build, commit, load") {
    const auto points = random_points(1000, 3);
    const auto uid    = unique_uid("load");

    PathName path;

    {
        TreeMappedCacheFile tree(uid, points.size());
        path = tree.path();
        remove(path);

        EXPECT(load_or_build(tree, points));
        EXPECT(path.exists());
        EXPECT(consistent(tree, points));
    }

    {
        TreeMappedCacheFile tree(uid, points.size());
        EXPECT_NOT(load_or_build(tree, points));
        EXPECT(consistent(tree, points));
    }

    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("TreeMappedFile: invalid file is rebuilt") {
    const auto points = random_points(100, 4);
    const auto uid    = unique_uid("invalid");

    TreeMappedCacheFile tree(uid, points.size());
    const auto path = tree.path();

    SECTION("garbage") {
        path.dirName().mkdir();
        std::ofstream(path.localPath()) << "garbage";
        EXPECT(path.exists());

        EXPECT(load_or_build(tree, points));
        EXPECT(consistent(tree, points));
    }

    SECTION("different size") {
        // same uid (path), different number of points
        const auto other = random_points(points.size() + 1, 5);
        {
            TreeMappedCacheFile tree_other(uid, other.size());
            EXPECT(load_or_build(tree_other, other));
        }

        EXPECT(load_or_build(tree, points));
        EXPECT(consistent(tree, points));
    }

    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("TreeMappedFile: uncommitted build is discarded") {
    const auto points = random_points(100, 6);
    const auto uid    = unique_uid("uncommitted");

    PathName path;
    {
        TreeMappedCacheFile tree(uid, points.size());
        path = tree.path();

        std::lock_guard<Tree> lock(tree);
        EXPECT_NOT(tree.ready());

        auto v = values(points);
        tree.build(v);

        EXPECT_EQUAL(count_temporary_files(path), 1);
    }

    EXPECT_NOT(path.exists());
    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("TreeMappedFile: multi-threaded") {
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
                TreeMappedCacheFile tree(uid, points.size());
                if (load_or_build(tree, points)) {
                    ++built;
                }
                if (!consistent(tree, points)) {
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

    TreeMappedCacheFile tree(uid, points.size());
    EXPECT_EQUAL(count_temporary_files(tree.path()), 0);
    remove(tree.path());
}


CASE("TreeMappedFile: multi-process") {
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
                TreeMappedCacheFile tree(uid, points.size());
                const bool built = load_or_build(tree, points);
                code             = consistent(tree, points) ? (built ? BUILT : LOADED) : FAILED;
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

    TreeMappedCacheFile tree(uid, points.size());
    EXPECT_NOT(load_or_build(tree, points));
    EXPECT(consistent(tree, points));

    EXPECT_EQUAL(count_temporary_files(tree.path()), 0);
    remove(tree.path());
}


CASE("Search: multi-threaded") {
    // unique points (uid), so the cached tree is new
    auto points = random_points(5000, 9);
    points.emplace_back(10., static_cast<double>(::getpid()),
                        static_cast<double>(std::chrono::steady_clock::now().time_since_epoch().count() % 1000000));

    const spec::Custom spec{{{"search-tree", "mapped-cache-file"}}};

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
    const auto& path = dynamic_cast<const TreeMappedFile&>(search.tree()).path();
    EXPECT(path.exists());
    EXPECT_EQUAL(count_temporary_files(path), 0);
    remove(path);
}


CASE("Search: caching") {
    const auto points = random_points(10, 10);

    // caching disabled: memory, unless explicitly requested
    EXPECT(dynamic_cast<const search::TreeMemory*>(&Search(points, spec::Custom{{{"caching", false}}}).tree()) !=
           nullptr);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
