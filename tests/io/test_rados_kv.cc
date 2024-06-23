/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include <cstring>

#include "eckit/eckit_config.h"
#include "eckit/config/Resource.h"

// #include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosKeyValue.h"
// #include "eckit/io/rados/RadosHandle.h"
// #include "eckit/io/rados/RadosMultiObjReadHandle.h"
// #include "eckit/io/rados/RadosMultiObjWriteHandle.h"

// #include "eckit/io/Buffer.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Rados KeyValue") {

#ifdef eckit_HAVE_RADOS_ADMIN
    std::string pool_name = "test_kv";
    std::string nspace = "default";
    RadosPool pool(pool_name);
    pool.ensureDestroyed();
    pool.ensureCreated();
#else
    std::string pool_name;
    std::string nspace = "test_kv";
    pool_name = eckit::Resource<std::string>(
        "eckitRadosTestPool;$ECKIT_RADOS_TEST_POOL", pool_name
    );
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

        // get
        char read_val[100] = "";
        res = kv.get(key, read_val, sizeof(read_val));
        EXPECT(res == val.size());
        EXPECT(std::string(read_val) == val);

        // list keys
        std::vector<std::string> keys = kv.keys();
        EXPECT(keys.size() == 2);
        EXPECT(keys.front() == key);
        EXPECT(keys.back() == key2);

        // has
        EXPECT(kv.has(key));
        EXPECT(kv.has(key2));
        EXPECT_NOT(kv.has("key3"));

        // remove
        kv.remove(key);
        kv.remove(key2);

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

#ifdef eckit_HAVE_RADOS_ADMIN
    pool.destroy();
#else
    RadosNamespace ns(pool_name, nspace);
    ns.destroy();
#endif

}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
