// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/MemoryCache.h"
#include "eckit/cache/Policy.h"
#include "eckit/testing/Test.h"


namespace eckit::cache::test {


using Entry = Policy::Entry;


/// a: small, old, used often; b: large, recent, used once
const Entry A{{100, 0}, 1, 5, 9};
const Entry B{{900, 0}, 6, 7, 1};


bool evicts_before(const std::string& policy, const Entry& a, const Entry& b) {
    const auto p = Policy::build(policy);
    EXPECT(p.name() == policy);
    return p.evicts_before(a, b);
}


CASE("Policy: which value is evicted first") {
    EXPECT(Policy::names() ==
           std::vector<std::string>({"fifo", "largest", "largest-per-use", "lfu", "lru", "mru", "used-once"}));

    EXPECT(evicts_before("lru", A, B));              // least recently used
    EXPECT(evicts_before("lfu", B, A));              // least frequently used
    EXPECT(evicts_before("fifo", A, B));             // first inserted
    EXPECT(evicts_before("largest", B, A));          // largest
    EXPECT(evicts_before("largest-per-use", B, A));  // largest per use
    EXPECT(evicts_before("mru", B, A));              // most recently used
    EXPECT(evicts_before("used-once", B, A));        // used once

    // ties: the least recently used
    const Entry C{{900, 0}, 2, 3, 1};
    EXPECT(evicts_before("largest", C, B));
    EXPECT(evicts_before("largest-per-use", C, B));
    EXPECT(evicts_before("lfu", C, B));
    EXPECT(evicts_before("used-once", C, B));

    // large but used often: largest evicts it first, largest-per-use the small value used less
    const Entry D{{900, 0}, 2, 3, 90};
    EXPECT(evicts_before("largest", D, A));
    EXPECT(evicts_before("largest-per-use", A, D));

    EXPECT_THROWS_AS(Policy::build("unknown"), CacheError);
}


CASE("MemoryCache: evicting by policy") {
    struct Value {
        explicit Value(size_t _bytes) : bytes(_bytes) {}
        MemoryUsage footprint() const { return {bytes, 0}; }
        const size_t bytes;
    };

    // capacity for 1000 bytes: values a (100), b (600), then c (400) is over
    auto evicted = [](const std::string& policy) {
        MemoryCaches caches;
        MemoryCache<Value> cache(caches, MemoryUsage{1000, 0}, policy);
        EXPECT(cache.policy().name() == policy);

        auto get = [&cache](const std::string& key, size_t bytes) {
            cache.get(key, MemoryUsage{bytes, 0}, [bytes] { return std::make_shared<const Value>(bytes); });
        };

        get("a", 100);
        get("b", 600);
        get("a", 100);  // a used again: more recently and more often than b
        get("c", 400);

        std::string evicted;
        for (const std::string key : {"a", "b"}) {
            bool made = false;
            cache.get(key, MemoryUsage{}, [&made] {
                made = true;
                return std::make_shared<const Value>(0);
            });
            if (made) {
                evicted += key;
            }
        }
        return evicted;
    };

    EXPECT(evicted("lru") == "b");              // b, least recently used
    EXPECT(evicted("lfu") == "b");              // b, used once
    EXPECT(evicted("fifo") == "a");             // a, inserted first
    EXPECT(evicted("largest") == "b");          // b, the largest
    EXPECT(evicted("largest-per-use") == "b");  // b, 600 bytes for one use (a, 100 bytes for two)
    EXPECT(evicted("mru") == "a");              // a, most recently used
    EXPECT(evicted("used-once") == "b");        // b, used once
}


}  // namespace eckit::cache::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
