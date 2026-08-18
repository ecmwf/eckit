/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Test Rados Handles") {

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    std::string pool_name = "test_handle";
    std::string nspace    = "default";
    RadosPool pool(pool_name);
    pool.ensureDestroyed();
    pool.ensureCreated();
#else
    std::string pool_name;
    std::string nspace = "test_handle";
    pool_name          = eckit::Resource<std::string>("eckitRadosTestPool;$ECKIT_RADOS_TEST_POOL", pool_name);
    EXPECT(pool_name.length() > 0);
    RadosPool pool(pool_name);
#endif

    SECTION("RadosHandle") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "foobar");

        RadosHandle h(obj);

        h.openForWrite(sizeof(buf));
        h.write(buf, sizeof(buf));
        h.close();

        Buffer mem(1024);
        RadosHandle g(obj);

        g.openForRead();
        g.read(mem, mem.size());
        g.close();

        EXPECT(buf == std::string(mem));

        obj.ensureDestroyed();
    }

    SECTION("RadosMultiObjWriteHandle") {

        const char buf[] =
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "foobar");

        RadosMultiObjWriteHandle h(obj, false, 16);

        h.openForWrite(sizeof(buf));
        h.write(buf, sizeof(buf));
        h.flush();
        h.close();

        Buffer mem(1024);
        RadosMultiObjReadHandle g(obj);

        g.openForRead();
        g.read(mem, mem.size());
        g.close();

        EXPECT(buf == std::string(mem));

        RadosCluster::instance().removeAll(obj);
    }

    SECTION("RadosMultiObjWriteHandle closes an exact-part write") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz012345";

        RadosObject obj(pool.name(), nspace, "exact-part");
        RadosMultiObjWriteHandle h(obj, false, 16);

        h.openForWrite(sizeof(buf) - 1);
        h.write(buf, sizeof(buf) - 1);
        h.close();

        Buffer mem(1024);
        RadosMultiObjReadHandle g(obj);

        g.openForRead();
        long read = g.read(mem, mem.size());
        g.close();

        EXPECT(read == sizeof(buf) - 1);
        EXPECT(std::memcmp(buf, mem.data(), read) == 0);

        RadosCluster::instance().removeAll(obj);
    }

    SECTION("RadosMultiObjWriteHandle closes asynchronous writes") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "async");
        RadosMultiObjWriteHandle h(obj, true, 16);

        h.openForWrite(sizeof(buf) - 1);
        h.write(buf, sizeof(buf) - 1);
        h.close();

        Buffer mem(1024);
        RadosMultiObjReadHandle g(obj);

        g.openForRead();
        long read = g.read(mem, mem.size());
        g.close();

        EXPECT(read == sizeof(buf) - 1);
        EXPECT(std::memcmp(buf, mem.data(), read) == 0);

        RadosCluster::instance().removeAll(obj);
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
