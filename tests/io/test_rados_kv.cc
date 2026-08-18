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
#include <vector>

#include <algorithm>

#include "eckit/config/Resource.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosKeyValue.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Rados KeyValue") {

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    std::string pool_name = "test_kv";
    std::string nspace    = "default";
    RadosPool pool(pool_name);
    pool.ensureDestroyed();
    pool.ensureCreated();
#else
    std::string pool_name;
    std::string nspace = "test_kv";
    pool_name          = eckit::Resource<std::string>("eckitRadosTestPool;$ECKIT_RADOS_TEST_POOL", pool_name);
    EXPECT(pool_name.length() > 0);
    RadosPool pool(pool_name);
#endif

    SECTION("RadosKeyValue operations") {

        std::string key = "key";
        std::string val = "abcdefghijklmnopqrstuvwxyz";

        std::string key2 = "key2";
        std::string val2 = "zyxwvutsrqponmlkjihgfedcba";

        RadosKeyValue kv(pool.name(), "default", "foobar");

        // create
        EXPECT_NOT(kv.exists());
        kv.ensureCreated();
        EXPECT(kv.exists());

        // put
        long res;
        res = kv.put(key, val.c_str(), val.size());
        EXPECT(res == val.size());
        res = kv.put(key2, val2.c_str(), val2.size());
        EXPECT(res == val2.size());

        std::string empty_key = "empty";
        res                   = kv.put(empty_key, nullptr, 0);
        EXPECT(res == 0);

        // get
        char read_val[100] = "";
        res                = kv.get(key, read_val, sizeof(read_val));
        EXPECT(res == val.size());
        EXPECT(std::string(read_val) == val);

        char empty_val = '\0';
        res            = kv.get(empty_key, &empty_val, sizeof(empty_val));
        EXPECT(res == 0);

        std::vector<char> empty_stream_data;
        eckit::MemoryStream empty_stream = kv.getMemoryStream(empty_stream_data, empty_key, "test kv");
        EXPECT(empty_stream.read(&empty_val, sizeof(empty_val)) == 0);
        EXPECT(empty_stream_data.empty());

        // list keys
        std::vector<std::string> keys = kv.keys();
        EXPECT(keys.size() == 3);
        EXPECT(std::find(keys.begin(), keys.end(), key) != keys.end());
        EXPECT(std::find(keys.begin(), keys.end(), key2) != keys.end());
        EXPECT(std::find(keys.begin(), keys.end(), empty_key) != keys.end());

        // has
        EXPECT(kv.has(key));
        EXPECT(kv.has(key2));
        EXPECT(kv.has(empty_key));
        EXPECT_NOT(kv.has("key3"));

        // remove
        kv.remove(key);
        kv.remove(key2);
        kv.remove(empty_key);

        // get non-existing key
        EXPECT_THROWS_AS(kv.get(key, read_val, sizeof(read_val)), eckit::RadosEntityNotFoundException);

        // list keys for empty kv
        keys.clear();
        keys = kv.keys();
        EXPECT(keys.size() == 0);

        // destroy kv
        EXPECT(kv.exists());
        kv.ensureDestroyed();
        EXPECT_NOT(kv.exists());
    }

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    pool.destroy();
#else
    RadosNamespace ns(pool_name, nspace);
    ns.destroy();
#endif
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
