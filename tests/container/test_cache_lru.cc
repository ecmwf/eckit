/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/container/CacheLRU.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

static size_t purgeCalls = 0;

static void purge(std::string& key, size_t& value) {
    Log::info() << "Purged key " << key << " with value " << value << std::endl;
    ++purgeCalls;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_cache_lru_basic") {
    eckit::CacheLRU<std::string, size_t> cache(3);

    EXPECT(cache.size() == 0);
    EXPECT(cache.capacity() == 3);

    // Add 2 items, check they're in the cache

    EXPECT(!cache.insert("ddd", 40));
    EXPECT(!cache.insert("aaa", 5));
    EXPECT(cache.insert("aaa", 10));

    EXPECT(cache.size() == 2);

    EXPECT(cache.exists("ddd"));
    EXPECT(cache.exists("aaa"));
    EXPECT(!cache.exists("bbb"));

    EXPECT(cache.access("aaa") == 10);
    EXPECT(cache.access("ddd") == 40);

    // Add 3 more items, displacing first 2

    EXPECT(!cache.insert("ccc", 30));
    EXPECT(!cache.insert("eee", 50));
    EXPECT(!cache.insert("bbb", 20));

    EXPECT(cache.size() == 3);

    EXPECT(cache.exists("bbb"));
    EXPECT(cache.exists("ccc"));
    EXPECT(cache.exists("eee"));

    EXPECT(cache.access("eee") == 50);
    EXPECT(cache.access("bbb") == 20);
    EXPECT(cache.access("ccc") == 30);

    EXPECT_THROWS_AS(cache.access("ddd"), eckit::OutOfRange);
    EXPECT_THROWS_AS(cache.access("aaa"), eckit::OutOfRange);

    // Increase capacity to 5, add 2 more items

    cache.capacity(5);
    EXPECT(cache.size() == 3);
    EXPECT(cache.capacity() == 5);

    EXPECT(!cache.insert("ddd", 40));
    EXPECT(!cache.insert("aaa", 10));

    EXPECT(cache.size() == 5);

    EXPECT(cache.exists("aaa"));
    EXPECT(cache.exists("bbb"));
    EXPECT(cache.exists("ccc"));
    EXPECT(cache.exists("ddd"));
    EXPECT(cache.exists("eee"));

    EXPECT(cache.access("aaa") == 10);
    EXPECT(cache.access("bbb") == 20);
    EXPECT(cache.access("ccc") == 30);
    EXPECT(cache.access("ddd") == 40);
    EXPECT(cache.access("eee") == 50);

    // Reduce capacity to 3, 2 items are purged

    cache.capacity(3);
    EXPECT(cache.size() == 3);
    EXPECT(cache.capacity() == 3);

    EXPECT(!cache.exists("aaa"));
    EXPECT(!cache.exists("bbb"));
    EXPECT(cache.exists("ccc"));
    EXPECT(cache.exists("ddd"));
    EXPECT(cache.exists("eee"));

    EXPECT_THROWS_AS(cache.access("aaa"), eckit::OutOfRange);
    EXPECT_THROWS_AS(cache.access("bbb"), eckit::OutOfRange);
    EXPECT(cache.access("ccc") == 30);
    EXPECT(cache.access("ddd") == 40);
    EXPECT(cache.access("eee") == 50);

    // Extract item

    EXPECT(cache.extract("ccc") == 30);
    EXPECT(cache.size() == 2);

    EXPECT(!cache.exists("ccc"));
    EXPECT(cache.exists("ddd"));
    EXPECT(cache.exists("eee"));

    EXPECT_THROWS_AS(cache.access("ccc"), eckit::OutOfRange);
    EXPECT(cache.access("ddd") == 40);
    EXPECT(cache.access("eee") == 50);

    // Remove item

    EXPECT(!cache.remove("ccc"));
    EXPECT(cache.remove("ddd"));
    EXPECT(cache.size() == 1);

    EXPECT(!cache.exists("ddd"));
    EXPECT_THROWS_AS(cache.access("ddd"), eckit::OutOfRange);

    // Clear the cache

    EXPECT_NO_THROW(cache.clear());
    EXPECT(cache.size() == 0);
    EXPECT(cache.capacity() == 3);

    EXPECT(!cache.exists("eee"));

    EXPECT_THROWS_AS(cache.access("eee"), eckit::OutOfRange);
}

CASE("test_cache_lru_purge") {
    eckit::CacheLRU<std::string, size_t> cache(4, purge);
    EXPECT(purgeCalls == 0);

    EXPECT(!cache.insert("aaa", 10));
    EXPECT(!cache.insert("bbb", 20));
    EXPECT(!cache.insert("ccc", 30));
    EXPECT(!cache.insert("ddd", 40));

    EXPECT(purgeCalls == 0);

    // Reducing capacity purges

    cache.capacity(3);
    EXPECT(purgeCalls == 1);

    // Extract does not purge

    EXPECT(cache.extract("bbb") == 20);
    EXPECT(purgeCalls == 1);

    // Remove purges

    EXPECT(cache.remove("ccc"));
    EXPECT(purgeCalls == 2);

    // Clear purges

    EXPECT_NO_THROW(cache.clear());
    EXPECT(purgeCalls == 3);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
