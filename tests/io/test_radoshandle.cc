// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstring>
#include <memory>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/rados/RadosAsyncHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
#include "eckit/io/rados/RadosObject.h"
#ifndef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
#include "eckit/io/rados/RadosNamespace.h"
#endif
#include "RadosTestUtils.h"
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

    SECTION("RadosAsyncHandle close waits for pending writes") {

        const std::string data(64 * 1024, 'x');

        RadosObject obj(pool.name(), nspace, "async-close");

        RadosAsyncHandle h(obj);
        h.openForWrite(0);
        for (int i = 0; i < 8; ++i) {
            h.write(data.data(), data.size());
        }
        h.close();

        EXPECT(RadosCluster::instance().size(obj) == Length(8 * data.size()));

        obj.ensureDestroyed();
    }

    SECTION("RadosAsyncHandle flushes when the pending data limit is reached") {

        const std::string data(4096, 'y');

        RadosObject obj(pool.name(), nspace, "async-limit");

        RadosAsyncHandle h(obj, 1024);
        h.openForWrite(0);
        for (int i = 0; i < 4; ++i) {
            h.write(data.data(), data.size());
        }
        h.close();

        EXPECT(RadosCluster::instance().size(obj) == Length(4 * data.size()));

        obj.ensureDestroyed();
    }

    SECTION("Asynchronous RadosMultiObjWriteHandle accepts several writes per part") {

        const std::string data(4096, 'z');

        RadosObject obj(pool.name(), nspace, "async-multi");

        std::unique_ptr<DataHandle> h(obj.asyncMultipartWriteHandle(10000));
        h->openForWrite(0);
        for (int i = 0; i < 5; ++i) {
            h->write(data.data(), data.size());
        }
        h->close();

        Buffer mem(5 * data.size());
        RadosMultiObjReadHandle g(obj);

        EXPECT(g.openForRead() == Length(5 * data.size()));
        long read = g.read(mem, mem.size());
        g.close();

        EXPECT(read == long(5 * data.size()));
        EXPECT(std::string(static_cast<const char*>(mem.data()), mem.size()) == std::string(5 * data.size(), 'z'));

        RadosCluster::instance().removeAll(obj);
    }

    SECTION("RadosAsyncHandle close reports a failed write") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj("eckit-test-no-such-pool", nspace, "async-fail");

        RadosAsyncHandle h(obj);
        h.openForWrite(0);
        EXPECT_THROWS(h.write(buf, sizeof(buf)));
        h.close();
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
    int ret = -1;
    try {
        ret = eckit::testing::run_tests(argc, argv);
    }
    catch (...) {
        eckit::Log::error() << "RADOS handle tests terminated with an exception" << std::endl;
    }

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    eckit::test::cleanupRados("test_handle", "default");
#else
    eckit::test::cleanupRados(eckit::test::configuredRadosTestPool(), "test_handle");
#endif

    return ret;
}
