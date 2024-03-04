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

CASE("RadosKeyValue") {

    std::string key = "key";
    std::string val = "abcdefghijklmnopqrstuvwxyz";

    RadosPool pool("mars");
    /// @todo: auto pool destroyer
    pool.ensureCreated();

    RadosKeyValue kv(pool.name(), "default", "foobar");

    // put
    long res;
    res = kv.put(key, val.c_str(), val.size());
    EXPECT(res == val.size());

    // get
    char read_val[100] = "";
    res = kv.get(key, read_val, sizeof(read_val));
    EXPECT(res == val.size());
    EXPECT(std::string(read_val) == val);

    // list keys
    std::vector<std::string> keys = kv.keys();
    EXPECT(keys.size() == 1);
    EXPECT(keys.front() == key);

    // remove
    kv.remove(key);

    // get non-existing key
    EXPECT_THROWS_AS(kv.get(key, read_val, sizeof(read_val)), eckit::RadosEntityNotFoundException);

    // list keys for empty kv
    keys.clear();
    keys = kv.keys();
    EXPECT(keys.size() == 0);

    // destroy kv
    kv.ensureDestroyed();

    pool.destroy();

}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
