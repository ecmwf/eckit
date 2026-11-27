// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <rados/librados.h>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "RadosTestUtils.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosAsyncKeyValue.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosKeyValue.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

std::string text(const std::optional<std::vector<char>>& value) {
    ASSERT(value);
    return {value->begin(), value->end()};
}

bool contains(const std::vector<std::string>& keys, const std::string& key) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("RadosKeyValue") {

    RadosTestSpace space("test_kv");

    RadosKeyValue kv(space.pool().name(), space.nspace(), "kv");

    SECTION("is created and destroyed") {
        EXPECT_NOT(kv.exists());
        kv.ensureCreated();
        EXPECT(kv.exists());
        kv.ensureCreated();
        EXPECT(kv.exists());

        kv.ensureDestroyed();
        EXPECT_NOT(kv.exists());
        kv.ensureDestroyed();
    }

    SECTION("stores, returns and removes values") {

        const std::string val  = "abcdefghijklmnopqrstuvwxyz";
        const std::string val2 = "zyxwvutsrqponmlkjihgfedcba";

        // put creates the key-value
        kv.put("key", val.data(), val.size());
        kv.put("key2", val2.data(), val2.size());
        kv.put("empty", nullptr, 0);
        EXPECT(kv.exists());

        EXPECT(text(kv.get("key")) == val);
        EXPECT(text(kv.get("key2")) == val2);
        EXPECT(text(kv.get("empty")).empty());
        EXPECT_NOT(kv.get("key3"));

        EXPECT(kv.has("key"));
        EXPECT(kv.has("empty"));
        EXPECT_NOT(kv.has("key3"));

        // a value is replaced
        kv.put("key", val2.data(), val2.size());
        EXPECT(text(kv.get("key")) == val2);

        // values are any bytes
        const std::string binary("a\0b\0", 4);
        kv.put("binary", binary.data(), binary.size());
        EXPECT(text(kv.get("binary")) == binary);

        kv.remove("key");
        EXPECT_NOT(kv.has("key"));
        EXPECT_NOT(kv.get("key"));

        // removing a key which is not there is not an error
        kv.remove("key");
    }

    SECTION("lists its keys, also over several queries") {

        EXPECT(kv.keys().empty());

        for (const std::string key : {"a", "b", "c", "d", "e"}) {
            kv.put(key, key.data(), key.size());
        }

        for (int keysPerQuery : {1000, 5, 2, 1}) {
            const auto keys = kv.keys(keysPerQuery);
            EXPECT(keys.size() == 5);
            for (const std::string key : {"a", "b", "c", "d", "e"}) {
                EXPECT(contains(keys, key));
            }
        }

        for (const std::string key : {"a", "b", "c", "d", "e"}) {
            kv.remove(key);
        }
        EXPECT(kv.keys().empty());
        EXPECT(kv.exists());
    }

    SECTION("has nothing if it does not exist") {
        EXPECT_NOT(kv.exists());
        EXPECT_NOT(kv.has("key"));
        EXPECT_NOT(kv.get("key"));
        EXPECT(kv.keys().empty());
    }

    SECTION("has a URI") {
        EXPECT(RadosKeyValue(kv.uri()).str() == kv.str());
        EXPECT(kv.object().str() == kv.str());
        EXPECT_THROWS_AS(RadosKeyValue(URI("rados:pool/name")), eckit::UserError);
        EXPECT_THROWS_AS(RadosKeyValue(space.pool().name(), "", "name").uri(), eckit::UserError);
        EXPECT_THROWS_AS(RadosKeyValue(space.pool().name(), space.nspace(), "kv#1"), eckit::UserError);
    }

    SECTION("refuses keys which are not text") {

        const std::string nul("a\0b", 3);
        const char one = '1';

        kv.ensureCreated();

        EXPECT_THROWS_AS(kv.put(nul, &one, 1), eckit::UserError);
        EXPECT_THROWS_AS(kv.has(nul), eckit::UserError);
        EXPECT_THROWS_AS(kv.get(nul), eckit::UserError);
        EXPECT_THROWS_AS(kv.remove(nul), eckit::UserError);
        EXPECT(kv.keys().empty());

        // such keys written by another client
        {
            const std::vector<std::string> foreign = {std::string("a\0b", 3), std::string("a\0c", 3), "zz"};
            std::vector<const char*> keys;
            std::vector<const char*> vals;
            std::vector<size_t> keyLens;
            std::vector<size_t> valLens;
            for (const auto& k : foreign) {
                keys.push_back(k.data());
                keyLens.push_back(k.size());
                vals.push_back(&one);
                valLens.push_back(1);
            }
            RadosWriteOp op;
            rados_write_op_omap_set2(op.op_, keys.data(), vals.data(), keyLens.data(), valLens.data(), keys.size());
            RADOS_CALL(rados_write_op_operate(op.op_, RadosCluster::instance().ioCtx(kv.object()), kv.name().c_str(),
                                              nullptr, 0));
        }

        // are listed when everything fits in one query, and reported, rather than looped on, otherwise
        EXPECT(kv.keys().size() == 3);
        EXPECT_THROWS_AS(kv.keys(1), eckit::UserError);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("RadosAsyncKeyValue") {

    RadosTestSpace space("test_kv");

    const std::string val = "abcdefghijklmnopqrstuvwxyz";

    SECTION("applies its modifications by the time flush returns") {

        // at most 2 modifications not waited for
        RadosAsyncKeyValue kv(space.pool().name(), space.nspace(), "async-kv", 2);

        kv.ensureCreated();
        for (const std::string key : {"a", "b", "c", "d", "e"}) {
            kv.put(key, val.data(), val.size());
        }
        kv.flush();

        EXPECT(kv.exists());
        EXPECT(kv.keys().size() == 5);
        EXPECT(text(kv.get("e")) == val);

        kv.remove("a");
        kv.remove("b");
        kv.flush();
        EXPECT(kv.keys().size() == 3);

        // nothing to wait for
        kv.flush();
    }

    SECTION("is asynchronous through the base class as well") {

        RadosAsyncKeyValue async(space.pool().name(), space.nspace(), "async-kv");
        RadosKeyValue& kv = async;

        kv.put("a", val.data(), val.size());
        kv.put("b", val.data(), val.size());
        kv.remove("a");
        async.flush();

        EXPECT(kv.keys().size() == 1);
        EXPECT(kv.has("b"));
    }

    SECTION("waits for what was not flushed when destroyed") {
        {
            RadosAsyncKeyValue kv(space.pool().name(), space.nspace(), "async-kv");
            for (const std::string key : {"a", "b", "c"}) {
                kv.put(key, val.data(), val.size());
            }
        }

        RadosKeyValue kv(space.pool().name(), space.nspace(), "async-kv");
        EXPECT(kv.keys().size() == 3);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
