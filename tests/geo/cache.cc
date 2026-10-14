// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <unistd.h>

#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "eckit/eckit_config.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/cache/Download.h"
#include "eckit/geo/cache/InMemoryCache.h"
#include "eckit/geo/cache/LatitudeLongitude.h"
#include "eckit/geo/cache/MemoryCache.h"
#include "eckit/geo/cache/MemoryUsage.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/geo/cache/Version.h"
#include "eckit/geo/search/FileIO.h"
#include "eckit/geo/search/KDTree.h"
#include "eckit/geo/search/MappedFile.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/thread/ThreadPool.h"


namespace eckit::geo::test {


/// Thread pool task, calling a function
template <typename Function>
class Task final : public ThreadPoolTask {
public:

    explicit Task(Function fun) : fun_(std::move(fun)) {}

private:

    void execute() override { fun_(); }

    Function fun_;
};


CASE("eckit::geo::util") {
    using Cache = cache::MemoryCache;

    struct test_t {
        size_t N;
        bool increasing;
        Cache::bytes_size_t pl_footprint;
        Cache::bytes_size_t pl_footprint_acc;
        Cache::bytes_size_t gl_footprint;
        Cache::bytes_size_t gl_footprint_acc;
    } tests[] = {
        {16, false, 256, 256, 256, 256},    //
        {24, false, 384, 640, 384, 640},    //
        {24, false, 384, 640, 384, 640},    // (repeated for a cache hit)
        {32, false, 512, 1152, 512, 1152},  //
        {16, false, 256, 1152, 256, 1152},  // (repeated for another cache hit)
        {48, false, 768, 1920, 768, 1920},  //
        {16, true, 256, 1920, 256, 2176},   // (repeated except for 'increasing')
        {24, true, 384, 1920, 384, 2560},   // ...
        {24, true, 384, 1920, 384, 2560},   //
        {32, true, 512, 1920, 512, 3072},   //
        {16, true, 256, 1920, 256, 3072},   //
        {48, true, 768, 1920, 768, 3840},   //
    };


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());


    SECTION("separate caches") {
        util::reduced_classical_pl(16);
        auto foot = Cache::total_footprint();
        EXPECT(0 < foot);

        util::reduced_octahedral_pl(16);
        EXPECT(foot < Cache::total_footprint());
    }


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());


    SECTION("reduced_classical_pl, reduced_octahedral_pl") {
        for (const auto& fun : {util::reduced_classical_pl, util::reduced_octahedral_pl}) {
            for (const auto& test : tests) {
                Cache::total_purge();
                fun(test.N);
                EXPECT(Cache::total_footprint() == test.pl_footprint);
            }

            Cache::total_purge();
            for (const auto& test : tests) {
                fun(test.N);
                EXPECT(Cache::total_footprint() == test.pl_footprint_acc);
            }
        }
    }


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());


    SECTION("gaussian_latitudes") {
        for (const auto& test : tests) {
            Cache::total_purge();
            util::gaussian_latitudes(test.N, test.increasing);
            EXPECT(Cache::total_footprint() == test.gl_footprint);
        }

        Cache::total_purge();
        for (const auto& test : tests) {
            util::gaussian_latitudes(test.N, test.increasing);
            EXPECT(Cache::total_footprint() == test.gl_footprint_acc);
        }
    }


    Cache::total_purge();
    EXPECT(0 == Cache::total_footprint());
}


#if eckit_HAVE_CURL
CASE("grid") {
    using Cache = cache::MemoryCache;

    const auto footprint_1 = Cache::total_footprint();

    spec::Custom spec({{"uid", "d5bde4f52ff3a9bea5629cd9ac514410"}});  // ORCA2_T
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec));

    // lazy behaviour is expected, force load only on calculate_uid
    Log::info() << "uid: '" << grid1->uid() << "'" << std::endl;

    const auto footprint_2 = Cache::total_footprint();
    EXPECT(footprint_1 == footprint_2);

#if eckit_HAVE_LZ4
    // calculate_uid requires the coordinates, which need uncompressing
    EXPECT(grid1->calculate_uid() == spec.get_string("uid"));
#endif

    const auto footprint_3 = Cache::total_footprint();
    EXPECT(footprint_2 <= footprint_3);

    std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("{uid:" + grid1->uid() + "}"));

    EXPECT(footprint_3 == Cache::total_footprint());
    EXPECT(grid1->size() == grid2->size());

    Cache::total_purge();
    EXPECT(Cache::total_footprint() <= footprint_1);
}
#endif


CASE("cache layout") {
    const PathName root{LibEcKitGeo::cacheDir()};

    // downloads (e.g. ORCA, ICON and FESOM coordinates) go to download/VERSION/<md5(url)>
    const cache::Download download;
    EXPECT(download.cache_root() == root / "download" / std::to_string(cache::version::DOWNLOAD));

    // coordinates written locally (e.g. unstructured grids) go to latlon/VERSION/<uid>.ek
    const auto path = cache::LatitudeLongitude({1., 2.}, {3., 4.}).to_cached_path();
    EXPECT(path.exists());
    EXPECT(path.dirName() == root / "latlon" / std::to_string(cache::version::LATLON));
}


CASE("MemoryUsage") {
    using cache::MemoryUsage;

    // as capacities are given: "memory[,shared]" (bytes), shared defaults to memory
    MemoryUsage a{"100,200"};
    MemoryUsage b{"300"};

    EXPECT(a.memory() == 100);
    EXPECT(a.shared() == 200);
    EXPECT(b.shared() == 300);

    EXPECT(a + b == MemoryUsage(400, 500));
    EXPECT(b - a == MemoryUsage(200, 100));
    EXPECT(a - b == MemoryUsage());  // saturates at zero
    EXPECT_NOT(MemoryUsage());

    // exceeding a capacity in either memory or shared memory
    EXPECT(MemoryUsage(301, 0).exceeds(b));
    EXPECT(MemoryUsage(0, 301).exceeds(b));
    EXPECT_NOT(b.exceeds(b));

    EXPECT_THROWS_AS(MemoryUsage{"1,2,3"}, BadValue);
}


CASE("InMemoryCache: usage, capacity and evictions") {
    using cache::InMemoryCache;
    using cache::MemoryUsage;
    using Cache = cache::MemoryCache;

    struct Values {
        explicit Values(size_t n) : values(n) {}
        MemoryUsage footprint() const { return {values.size() * sizeof(double), 0}; }
        std::vector<double> values;
    };

    // 50 values use 400 bytes, the capacity is 1000 bytes (in process memory)
    InMemoryCache<Values> cached(MemoryUsage{1000, 0});

    // n values, as estimated
    auto get = [&cached](const std::string& key, size_t n) {
        return cached.get(key, MemoryUsage{n * sizeof(double), 0}, [n]() { return std::make_shared<const Values>(n); });
    };

    const auto total = Cache::total_footprint();

    get("a", 50);  // miss
    get("b", 50);  // miss
    get("a", 50);  // hit, "a" is now more recently used than "b"

    EXPECT(cached.usage() == MemoryUsage(800, 0));
    EXPECT(cached.statistics().hits == 1);
    EXPECT(cached.statistics().misses == 2);

    // reported in the memory caches total
    EXPECT(Cache::total_footprint() == total + 800);

    // over capacity: the least recently used is evicted
    get("c", 50);
    EXPECT(cached.size() == 2);
    EXPECT(cached.usage() == MemoryUsage(800, 0));
    EXPECT(cached.statistics().evictions == 1);

    // values in use are not evicted, even over capacity
    auto d = get("d", 200);
    EXPECT(cached.size() == 1);
    EXPECT(cached.usage() == MemoryUsage(1600, 0));

    d.reset();
    cached.capacity(MemoryUsage{1000, 0});
    EXPECT(cached.size() == 0);
    EXPECT(Cache::total_footprint() == total);
}


/// Cached value of a given footprint
struct Footprint {
    explicit Footprint(const cache::MemoryUsage& _usage) : usage(_usage) {}
    cache::MemoryUsage footprint() const { return usage; }
    const cache::MemoryUsage usage;
};


CASE("InMemoryCache: room is made before making a value") {
    using cache::InMemoryCache;
    using cache::MemoryUsage;

    InMemoryCache<Footprint> cached(MemoryUsage{1000, 1000});

    size_t made = 0;
    auto make   = [&made](const MemoryUsage& usage) {
        return [&made, usage]() {
            ++made;
            return std::make_shared<const Footprint>(usage);
        };
    };

    std::weak_ptr<const Footprint> a = cached.get("a", MemoryUsage{400, 0}, make({400, 0}));
    std::weak_ptr<const Footprint> b = cached.get("b", MemoryUsage{400, 0}, make({400, 0}));
    std::weak_ptr<const Footprint> s = cached.get("s", MemoryUsage{0, 800}, make({0, 800}));
    EXPECT(cached.usage() == MemoryUsage(800, 800));
    EXPECT(made == 3);

    SECTION("a cached value is not made again, and nothing is evicted (even if the estimate doesn't fit)") {
        EXPECT(cached.get("a", MemoryUsage{1000, 0}, make({400, 0})).get() == a.lock().get());
        EXPECT(made == 3);
        EXPECT(cached.statistics().hits == 1);
        EXPECT(cached.statistics().evictions == 0);
        EXPECT(cached.size() == 3);
    }

    SECTION("evicting before making, only what relieves the capacity (memory)") {
        cached.get("c", MemoryUsage{400, 0}, [&]() {
            // the least recently used, before c is made
            EXPECT(a.expired());
            EXPECT_NOT(b.expired());
            EXPECT_NOT(s.expired());  // in shared memory
            return make({400, 0})();
        });

        EXPECT(cached.statistics().evictions == 1);
        EXPECT(cached.usage() == MemoryUsage(800, 800));
    }

    SECTION("evicting before making, only what relieves the capacity (shared)") {
        cached.get("t", MemoryUsage{0, 400}, [&]() {
            EXPECT_NOT(a.expired());
            EXPECT_NOT(b.expired());
            EXPECT(s.expired());
            return make({0, 400})();
        });

        EXPECT(cached.usage() == MemoryUsage(800, 400));
    }

    SECTION("a value beyond the capacity is made and cached, evicting all values not in use") {
        auto in_use = b.lock();

        auto big = cached.get("big", MemoryUsage{2000, 0}, [&]() {
            EXPECT(a.expired());
            EXPECT_NOT(b.expired());
            return make({2000, 0})();
        });

        EXPECT(cached.size() == 3);  // b, s and big
        EXPECT(cached.usage() == MemoryUsage(2400, 800));

        // evicted only once not in use
        in_use.reset();
        big.reset();
        cached.capacity(MemoryUsage{1000, 1000});
        EXPECT(cached.size() == 1);
        EXPECT(cached.usage() == MemoryUsage(0, 800));
    }

    SECTION("an estimate below the footprint, evicting after making") {
        auto c = cached.get("c", MemoryUsage{100, 0}, make({400, 0}));
        EXPECT(a.expired());
        EXPECT_NOT(b.expired());
        EXPECT(cached.usage() == MemoryUsage(800, 800));
    }
}


CASE("InMemoryCache: a value is made once, if requested concurrently") {
    using cache::InMemoryCache;
    using cache::MemoryUsage;

    InMemoryCache<Footprint> cached(MemoryUsage{1000, 0});

    constexpr size_t N = 4;

    util::recursive_mutex mutex;
    int made = 0;

    std::vector<std::shared_ptr<const Footprint>> values(N);  // per task

    ThreadPool pool("threads", N);
    for (size_t i = 0; i < N; ++i) {
        pool.push(new Task([&, i]() {
            values[i] = cached.get("x", MemoryUsage{100, 0}, [&]() {
                {
                    util::lock_guard<util::recursive_mutex> lock(mutex);
                    ++made;
                }
                ::usleep(10000);  // so the other requests are concurrent
                return std::make_shared<const Footprint>(MemoryUsage{100, 0});
            });
        }));
    }
    pool.waitForThreads();  // throws if a task failed

    EXPECT(made == 1);
    for (const auto& value : values) {
        EXPECT(value == values.front());
    }

    EXPECT(cached.statistics().misses == 1);
    EXPECT(cached.statistics().hits == N - 1);
    EXPECT(cached.size() == 1);
}


CASE("InMemoryCache: a value failing to be made is not cached") {
    using cache::InMemoryCache;
    using cache::MemoryUsage;

    InMemoryCache<Footprint> cached(MemoryUsage{1000, 0});

    auto make = [](const MemoryUsage& usage) { return [usage]() { return std::make_shared<const Footprint>(usage); }; };

    std::weak_ptr<const Footprint> a = cached.get("a", MemoryUsage{400, 0}, make({400, 0}));

    auto failing = []() -> std::shared_ptr<const Footprint> { throw std::runtime_error("failed"); };
    EXPECT_THROWS_AS(cached.get("x", MemoryUsage{600, 0}, failing), std::runtime_error);
    EXPECT(cached.size() == 1);

    // the room made is not kept: a fits along with b
    cached.get("b", MemoryUsage{600, 0}, make({600, 0}));
    EXPECT_NOT(a.expired());
    EXPECT(cached.statistics().evictions == 0);

    // made on the next request
    cached.get("x", MemoryUsage{0, 0}, make({0, 0}));
    EXPECT(cached.size() == 3);
    EXPECT(cached.statistics().misses == 4);
}


CASE("SearchCache: a grid search, accounted for") {
    auto& cache = cache::SearchCache::instance();
    cache.clear();

    const auto capacity = cache.capacity();

    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));

    // a grid holds its search (default configuration), loading it increases the cache usage
    const auto& search = grid->search();
    EXPECT(search.footprint());
    EXPECT(cache.usage() == search.footprint());
    EXPECT(&search == &grid->search());

    // default loader (mapped-memory), as mir: the cache file mapped, using shared memory (page cache)
    if (LibEcKitGeo::caching()) {
        EXPECT(dynamic_cast<const search::MappedFile*>(&search.kdtree()) != nullptr);
        EXPECT(search.footprint().shared() > 0);
    }

    // searches are cached by grid and k-d tree configuration
    auto memory = cache.get(*grid, spec::Custom{{{"search-loader", "memory"}}});
    EXPECT(memory.get() != &search);
    EXPECT(memory->footprint().memory() > 0);
    EXPECT(cache.usage() == search.footprint() + memory->footprint());
    EXPECT(cache.get(*grid, spec::Custom{{{"search-loader", "memory"}}}) == memory);

    // other behaviours share the k-d tree, so they are not accounted for again
    auto knn = cache.get(*grid, spec::Custom{{{"search-loader", "memory"}, {"search", "knn"}, {"search-k", 4}}});
    EXPECT(&knn->kdtree() == &memory->kdtree());
    EXPECT(knn->search(PointLonLat{14., 1.}).size() == 4);
    EXPECT(cache.size() == 2);

    // searches in use are not evicted (the grid's own, and the other shared by knn)
    memory.reset();
    cache.capacity(cache::MemoryUsage{0, 0});
    EXPECT(cache.size() == 2);

    knn.reset();
    cache.capacity(cache::MemoryUsage{0, 0});
    EXPECT(cache.size() == 1);
    EXPECT(cache.usage() == search.footprint());

    cache.capacity(capacity);
    cache.clear();
}


CASE("SearchCache: room is made for a search before loading it") {
    auto& cache = cache::SearchCache::instance();
    cache.clear();

    const auto capacity = cache.capacity();

    std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));
    std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom{{{"grid", "20/20"}}}));

    // in process memory, as estimated
    const spec::Custom spec{{{"search-loader", "file-io"}}};

    auto search1 = cache.get(*grid1, spec);
    EXPECT(dynamic_cast<const search::FileIO*>(&search1->kdtree()) != nullptr);
    EXPECT(search1->footprint() == search::KDTreeFactory::estimate("file-io", grid1->size()));
    EXPECT(cache.usage() == search1->footprint());

    // room for one search only
    const auto estimate2 = search::KDTreeFactory::estimate("file-io", grid2->size());
    cache.capacity(estimate2 + cache::MemoryUsage{0, capacity.shared()});

    std::weak_ptr<const Search> evicted = search1;
    search1.reset();

    const auto evictions = cache.statistics().evictions;
    auto search2         = cache.get(*grid2, spec);
    EXPECT(evicted.expired());
    EXPECT(cache.statistics().evictions == evictions + 1);
    EXPECT(cache.usage() == estimate2);

    // cached, so nothing is evicted
    EXPECT(cache.get(*grid2, spec) == search2);
    EXPECT(cache.statistics().evictions == evictions + 1);

    search2.reset();
    cache.capacity(capacity);
    cache.clear();
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
