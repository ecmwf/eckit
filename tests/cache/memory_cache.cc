// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "eckit/cache/MemoryCache.h"
#include "eckit/cache/MemoryUsage.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


CASE("MemoryUsage") {

    // "memory[,shared]" (bytes), shared defaults to 0
    MemoryUsage a{"100,200"};
    MemoryUsage b{"300"};

    EXPECT(a.memory() == 100);
    EXPECT(a.shared() == 200);
    EXPECT(b.memory() == 300);
    EXPECT(b.shared() == 0);

    EXPECT(a + b == MemoryUsage(400, 200));
    EXPECT(b - a == MemoryUsage(200, 0));  // saturates at zero
    EXPECT(a - a == MemoryUsage());
    EXPECT_NOT(MemoryUsage());

    // exceeding a capacity in either memory or shared memory
    EXPECT(MemoryUsage(301, 0).exceeds(b));
    EXPECT(MemoryUsage(0, 1).exceeds(b));
    EXPECT_NOT(b.exceeds(b));

    EXPECT_THROWS_AS(MemoryUsage{"1,2,3"}, BadValue);
}


CASE("MemoryCache: usage, capacity and evictions") {

    struct Values {
        explicit Values(size_t n) : values(n) {}
        MemoryUsage footprint() const { return {values.size() * sizeof(double), 0}; }
        std::vector<double> values;
    };

    // 50 values use 400 bytes, the capacity is 1000 bytes (in process memory)
    MemoryCaches caches;
    MemoryCache<Values> cached(caches, MemoryUsage{1000, 0});

    // n values, as estimated
    auto get = [&cached](const std::string& key, size_t n) {
        return cached.get(key, MemoryUsage{n * sizeof(double), 0}, [n]() { return std::make_shared<const Values>(n); });
    };

    get("a", 50);  // miss
    get("b", 50);  // miss
    get("a", 50);  // hit, "a" is now more recently used than "b"

    EXPECT(cached.usage() == MemoryUsage(800, 0));
    EXPECT(cached.statistics().hits == 1);
    EXPECT(cached.statistics().misses == 2);

    // reported in its group of caches
    EXPECT(caches.footprint() == MemoryUsage(800, 0));

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
    EXPECT(caches.footprint() == MemoryUsage());
}


/// Cached value of a given footprint
struct Footprint {
    explicit Footprint(const MemoryUsage& _usage) : usage(_usage) {}
    MemoryUsage footprint() const { return usage; }
    const MemoryUsage usage;
};


CASE("MemoryCache: room is made before making a value") {

    MemoryCaches caches;
    MemoryCache<Footprint> cached(caches, MemoryUsage{1000, 1000});

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


CASE("MemoryCache: a value failing to be made is not cached") {

    MemoryCaches caches;
    MemoryCache<Footprint> cached(caches, MemoryUsage{1000, 0});

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


CASE("MemoryCache: values without footprint(), erased, and statistics") {
    // e.g. a type of another library, its footprint known by the user
    MemoryCaches caches;
    MemoryCache<std::vector<double>> cached(caches, MemoryUsage{1000, 0});

    auto make      = [](size_t n) { return [n]() { return std::make_shared<const std::vector<double>>(n); }; };
    auto footprint = [](const std::vector<double>& v) { return MemoryUsage{v.size() * sizeof(double), 0}; };

    auto a = cached.get("a", MemoryUsage{80, 0}, make(10), footprint);
    EXPECT(a->size() == 10);
    EXPECT(cached.usage() == MemoryUsage(80, 0));
    EXPECT(cached.get("a", MemoryUsage{80, 0}, make(10), footprint) == a);  // cached

    cached.get("b", MemoryUsage{160, 0}, make(20), footprint);
    EXPECT(cached.statistics().insertions == 2);
    EXPECT(cached.statistics().hits == 1);
    EXPECT(cached.statistics().misses == 2);

    // erased, though valid for its user
    cached.erase("a");
    EXPECT(cached.size() == 1);
    EXPECT(cached.statistics().evictions == 1);
    EXPECT(a->size() == 10);

    EXPECT(cached.get("a", MemoryUsage{80, 0}, make(10), footprint) != a);  // made again
    EXPECT(cached.statistics().insertions == 3);
}


CASE("MemoryCaches: footprint and purge, of a group of caches") {
    struct Cache final : MemoryCacheBase {
        explicit Cache(MemoryCaches& group) : MemoryCacheBase(group) { register_cache(); }
        ~Cache() override { unregister_cache(); }
        MemoryUsage footprint() const override { return usage; }
        void purge() override { usage = {}; }
        MemoryUsage usage{10, 1};
    };

    MemoryCaches library;
    MemoryCaches other;

    Cache a(library);
    Cache b(library);
    Cache c(other);

    EXPECT(library.footprint() == MemoryUsage(20, 2));
    EXPECT(other.footprint() == MemoryUsage(10, 1));

    // only the caches of the group
    library.purge();
    EXPECT(library.footprint() == MemoryUsage());
    EXPECT(c.footprint() == MemoryUsage(10, 1));

    // not in the group once destructed
    {
        Cache d(other);
        EXPECT(other.footprint() == MemoryUsage(20, 2));
    }
    EXPECT(other.footprint() == MemoryUsage(10, 1));
}


CASE("MemoryCacheT: values by key, returned by reference") {
    using cache_t = MemoryCacheT<int, std::vector<double>>;
    MemoryCaches caches;
    cache_t cached(caches);

    EXPECT(cached.find(1) == nullptr);

    const auto& a = cached.insert(1, std::vector<double>(10));
    EXPECT(cached.find(1) == &a);

    // inserting a cached key keeps the cached value (e.g. made concurrently)
    EXPECT(&cached.insert(1, std::vector<double>(20)) == &a);
    EXPECT(a.size() == 10);

    // footprint: size() elements, if the value has no footprint()
    cached.insert(2, std::vector<double>(5));
    EXPECT(cached.footprint() == MemoryUsage(15 * sizeof(double), 0));
    EXPECT(caches.footprint() == cached.footprint());

    caches.purge();
    EXPECT(cached.find(1) == nullptr);
    EXPECT(cached.footprint() == MemoryUsage());
}


CASE("MemoryCacheT: footprint(), of process and shared memory (as MemoryCache values)") {
    struct Record {
        MemoryUsage footprint() const { return {42, 1}; }
    };

    MemoryCaches caches;
    MemoryCacheT<std::string, Record> cached(caches);
    cached.insert("a", Record{});
    cached.insert("b", Record{});
    EXPECT(cached.footprint() == MemoryUsage(84, 2));
}


CASE("MemoryCacheT: concurrent users of a key get the same value") {
    MemoryCaches caches;
    MemoryCacheT<int, std::vector<double>> cached(caches);

    std::vector<const std::vector<double>*> values(8);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < values.size(); ++i) {
        threads.emplace_back([&cached, &values, i]() {
            const auto* cached_value = cached.find(1);
            values[i] = cached_value != nullptr ? cached_value : &cached.insert(1, std::vector<double>(i + 1));
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT(std::set<const std::vector<double>*>(values.begin(), values.end()).size() == 1);
    EXPECT(values.front() == cached.find(1));
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
