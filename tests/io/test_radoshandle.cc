// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <rados/librados.h>

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "RadosTestUtils.h"
#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosAsyncHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
#include "eckit/io/rados/RadosNamespace.h"
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
    if (pool_name.empty()) {
        throw eckit::UserError("The RADOS tests need the name of an existing pool in ECKIT_RADOS_TEST_POOL", Here());
    }
    RadosPool pool(pool_name);
#endif

    SECTION("ensureCreated accepts an existing pool") {
        EXPECT(pool.exists());
        pool.ensureCreated();
        pool.ensureCreated();
        EXPECT(pool.exists());
        EXPECT_NOT(RadosPool("eckit-test-no-such-pool").exists());
    }

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

    SECTION("RadosHandle openForWrite empties an existing object") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "truncate");

        RadosHandle h(obj);
        h.openForWrite(0);
        h.write(buf, sizeof(buf));
        h.close();
        EXPECT(h.size() == Length(sizeof(buf)));

        // nothing written: the object exists and is empty
        h.openForWrite(0);
        h.close();
        EXPECT(obj.exists());
        EXPECT(h.size() == Length(0));

        // a shorter rewrite leaves nothing of the previous content
        h.openForWrite(0);
        EXPECT(h.write(buf, 0) == 0);
        h.write(buf, 4);
        h.close();
        EXPECT(h.size() == Length(4));

        obj.ensureDestroyed();
    }

    SECTION("RadosHandle seek and rewind") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "seek");

        RadosHandle h(obj);

        // previous, longer content
        h.openForWrite(0);
        h.write(buf, 26);
        h.write(buf, 26);
        h.close();
        EXPECT(h.size() == Length(52));

        // seek before the first write is honoured, and nothing of the previous content is left
        h.openForWrite(0);
        EXPECT(h.seek(4) == Offset(4));
        h.write(buf, 26);
        EXPECT(h.position() == Offset(30));
        h.close();
        EXPECT(h.size() == Length(30));

        char out[8] = {};
        h.openForRead();
        EXPECT(h.seek(14) == Offset(14));
        EXPECT(h.read(out, 3) == 3);
        EXPECT(std::string(out, 3) == "klm");
        h.rewind();
        EXPECT(h.position() == Offset(0));
        EXPECT(h.seek(4) == Offset(4));
        EXPECT(h.read(out, 3) == 3);
        EXPECT(std::string(out, 3) == "abc");
        h.close();

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

    SECTION("RadosMultiObjWriteHandle splits a write larger than the maximum write size") {

        const long size = static_cast<long>(RadosCluster::instance().maxWriteSize()) + 1024 * 1024;
        Buffer data(size);
        std::memset(data.data(), 'w', size);

        RadosObject obj(pool.name(), nspace, "large-write");

        std::unique_ptr<DataHandle> h(obj.multipartWriteHandle());
        h->openForWrite(0);
        EXPECT(h->write(data.data(), size) == size);
        h->close();

        EXPECT(RadosCluster::instance().size(obj) == Length(size));

        obj.ensureAllDestroyed();
    }

    SECTION("Asynchronous RadosMultiObjWriteHandle writes more parts than handles it may hold") {

        const std::string data(100, 'q');

        RadosObject obj(pool.name(), nspace, "async-many-parts");

        // 4 KiB of pending data per part, at most 3 open handles, 13 parts
        std::unique_ptr<DataHandle> h(obj.asyncMultipartWriteHandle(8, 4096, 3));
        h->openForWrite(0);
        h->write(data.data(), 50);
        h->flush();
        h->write(data.data() + 50, 50);
        h->close();

        EXPECT(RadosObject(obj, 12).exists());

        Buffer mem(data.size());
        RadosMultiObjReadHandle g(obj);
        EXPECT(g.openForRead() == Length(data.size()));
        EXPECT(g.read(mem, mem.size()) == long(data.size()));
        g.close();
        EXPECT(std::string(static_cast<const char*>(mem.data()), mem.size()) == data);

        obj.ensureAllDestroyed();
    }

    SECTION("RadosMultiObjWriteHandle stores an empty object") {

        RadosObject obj(pool.name(), nspace, "empty-multi");

        std::unique_ptr<DataHandle> h(obj.multipartWriteHandle(16));
        h->openForWrite(0);
        h->close();

        EXPECT(obj.exists());

        char c = 0;
        RadosMultiObjReadHandle g(obj);
        EXPECT(g.openForRead() == Length(0));
        EXPECT(g.read(&c, 1) == 0);
        g.close();

        obj.ensureAllDestroyed();
        EXPECT_NOT(obj.exists());
    }

    SECTION("RadosMultiObjWriteHandle replaces an object which had more parts") {

        const std::string data(100, 'r');

        RadosObject obj(pool.name(), nspace, "rewrite");

        std::unique_ptr<DataHandle> h(obj.multipartWriteHandle(16));
        h->openForWrite(0);
        h->write(data.data(), data.size());
        h->close();
        EXPECT(RadosObject(obj, 6).exists());

        h->openForWrite(0);
        h->write(data.data(), 20);
        h->close();
        EXPECT(RadosObject(obj, 1).exists());
        for (size_t i = 2; i < 7; ++i) {
            EXPECT_NOT(RadosObject(obj, i).exists());
        }

        RadosMultiObjReadHandle g(obj);
        EXPECT(g.openForRead() == Length(20));
        g.close();

        obj.ensureAllDestroyed();
    }

    SECTION("The empty namespace is the default namespace") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject inDefault(pool.name(), "", "eckit-test-default-ns");
        RadosObject inNamed(pool.name(), nspace, "eckit-test-named-ns");

        for (const auto& obj : {inDefault, inNamed}) {
            RadosHandle h(obj);
            h.openForWrite(0);
            h.write(buf, sizeof(buf));
            h.close();
        }

        EXPECT(inDefault.exists());
        EXPECT_NOT(RadosObject(pool.name(), nspace, "eckit-test-default-ns").exists());

        auto names = [](const std::vector<RadosObject>& objects) {
            std::vector<std::string> res;
            for (const auto& o : objects) {
                res.push_back(o.name());
            }
            return res;
        };

        // each namespace only lists its own objects
        auto inDefaultNs = names(RadosNamespace(pool.name(), "").listObjects());
        EXPECT(std::find(inDefaultNs.begin(), inDefaultNs.end(), "eckit-test-default-ns") != inDefaultNs.end());
        EXPECT(std::find(inDefaultNs.begin(), inDefaultNs.end(), "eckit-test-named-ns") == inDefaultNs.end());

        auto inNamedNs = names(RadosNamespace(pool.name(), nspace).listObjects());
        EXPECT(std::find(inNamedNs.begin(), inNamedNs.end(), "eckit-test-named-ns") != inNamedNs.end());
        EXPECT(std::find(inNamedNs.begin(), inNamedNs.end(), "eckit-test-default-ns") == inNamedNs.end());

        auto namespaces = pool.listNamespaces();
        EXPECT(std::find(namespaces.begin(), namespaces.end(), "") != namespaces.end());
        EXPECT(std::find(namespaces.begin(), namespaces.end(), nspace) != namespaces.end());

        inDefault.ensureDestroyed();
        inNamed.ensureDestroyed();
        EXPECT_NOT(inDefault.exists());
    }

    SECTION("Range reads") {

        std::string data;
        for (int i = 0; i < 100; ++i) {
            data += static_cast<char>('a' + i % 26);
        }

        // 7 parts of at most 16 bytes
        RadosObject multi(pool.name(), nspace, "range-multi");
        {
            std::unique_ptr<DataHandle> h(multi.multipartWriteHandle(16));
            h->openForWrite(0);
            h->write(data.data(), data.size());
            h->close();
        }

        RadosObject single(pool.name(), nspace, "range-single");
        {
            std::unique_ptr<DataHandle> h(single.dataHandle());
            h->openForWrite(0);
            h->write(data.data(), data.size());
            h->close();
        }

        char out[128];

        // seek and read across the parts of a multipart object
        {
            RadosMultiObjReadHandle g(multi);
            EXPECT(g.size() == Length(100));
            EXPECT(g.openForRead() == Length(100));

            EXPECT(g.seek(10) == Offset(10));
            EXPECT(g.read(out, 30) == 30);
            EXPECT(std::string(out, 30) == data.substr(10, 30));
            EXPECT(g.position() == Offset(40));

            EXPECT(g.seek(96) == Offset(96));
            EXPECT(g.read(out, 30) == 4);
            EXPECT(std::string(out, 4) == data.substr(96));
            EXPECT(g.read(out, 30) == 0);

            g.rewind();
            EXPECT(g.read(out, sizeof(out)) == 100);
            EXPECT(std::string(out, 100) == data);
            g.close();
        }

        // a range of a multipart object, starting and ending inside parts
        {
            std::unique_ptr<DataHandle> g(multi.multipartRangeReadHandle(13, 40));
            EXPECT(g->openForRead() == Length(40));
            long n = g->read(out, sizeof(out));
            EXPECT(n == 40);
            EXPECT(std::string(out, n) == data.substr(13, 40));
            g->close();
        }

        // a range of a single object
        {
            std::unique_ptr<DataHandle> g(single.rangeReadHandle(13, 40));
            EXPECT(g->openForRead() == Length(40));
            EXPECT(g->read(out, 10) == 10);
            EXPECT(std::string(out, 10) == data.substr(13, 10));
            EXPECT(g->position() == Offset(10));
            EXPECT(g->seek(35) == Offset(35));
            EXPECT(g->read(out, sizeof(out)) == 5);
            EXPECT(std::string(out, 5) == data.substr(48, 5));
            EXPECT(g->read(out, sizeof(out)) == 0);

            // rewinding goes back to the start of the range
            g->rewind();
            EXPECT(g->position() == Offset(0));
            EXPECT(g->read(out, 10) == 10);
            EXPECT(std::string(out, 10) == data.substr(13, 10));
            g->close();

            // so does opening the handle again
            EXPECT(g->openForRead() == Length(40));
            EXPECT(g->position() == Offset(0));
            EXPECT(g->read(out, sizeof(out)) == 40);
            EXPECT(std::string(out, 40) == data.substr(13, 40));
            g->close();
        }

        // a read handle opened again sees an object which was replaced in between
        {
            RadosMultiObjReadHandle g(multi);
            EXPECT(g.openForRead() == Length(100));
            g.close();

            std::unique_ptr<DataHandle> h(multi.multipartWriteHandle(8));
            h->openForWrite(0);
            h->write(data.data(), 20);
            h->close();

            EXPECT(g.size() == Length(20));
            EXPECT(g.openForRead() == Length(20));
            EXPECT(g.read(out, sizeof(out)) == 20);
            EXPECT(std::string(out, 20) == data.substr(0, 20));
            g.close();
        }

        multi.ensureAllDestroyed();
        single.ensureAllDestroyed();
    }

    SECTION("rados URIs") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "uri-object");

        URI uri = obj.uri();
        EXPECT(uri.scheme() == "rados");
        EXPECT(uri.asString() == "rados:" + pool.name() + "/" + nspace + "/uri-object");
        EXPECT(RadosObject(uri).str() == obj.str());
        EXPECT(RadosObject(URI(uri.asString())).str() == obj.str());

        EXPECT_NOT(uri.exists());

        std::unique_ptr<DataHandle> h(uri.newWriteHandle());
        h->openForWrite(0);
        h->write(buf, sizeof(buf));
        h->close();

        EXPECT(uri.exists());

        char out[64] = {};
        std::unique_ptr<DataHandle> g(uri.newReadHandle());
        EXPECT(g->openForRead() == Length(sizeof(buf)));
        EXPECT(g->read(out, sizeof(out)) == long(sizeof(buf)));
        g->close();
        EXPECT(std::string(out) == buf);

        EXPECT(RadosNamespace(obj.nspace().uri()).str() == obj.nspace().str());
        EXPECT(RadosPool(pool.uri()).name() == pool.name());

        obj.ensureDestroyed();
        EXPECT_NOT(uri.exists());

        // the default namespace has no URI form
        EXPECT_THROWS_AS(RadosObject(pool.name(), "", "name").uri(), eckit::UserError);
        EXPECT_THROWS_AS(RadosNamespace(pool.name(), "").uri(), eckit::UserError);
        EXPECT(RadosObject(pool.name(), "", "name").str() == pool.name() + "//name");

        // names which a URI would turn into another name, or which librados would cut short, are refused
        for (const std::string bad : {std::string("entry#1"), std::string("entry?1"), std::string("x/../y"),
                                      std::string("."), std::string(".."), std::string("a\0b", 3)}) {
            EXPECT_THROWS_AS(RadosObject(pool.name(), nspace, bad), eckit::UserError);
            EXPECT_THROWS_AS(RadosNamespace(pool.name(), bad), eckit::UserError);
            EXPECT_THROWS_AS(RadosPool{bad}, eckit::UserError);
        }
        EXPECT_THROWS_AS(RadosObject(URI("rados:pool/ns/..")), eckit::UserError);

        // other punctuation is fine and survives the URI
        for (const std::string good : {"a b", "a:b", "a%20b", "a.b", "a;b", "..."}) {
            RadosObject o(pool.name(), nspace, good);
            EXPECT(RadosObject(URI(o.uri().asString())).str() == o.str());
        }

        // an object with such a name, written by another tool, is left out of a listing
        {
            const std::string foreign = "written#elsewhere";
            RadosObject visible(pool.name(), nspace, "visible");
            rados_ioctx_t& ctx = RadosCluster::instance().ioCtx(visible);

            RADOS_CALL(rados_write_full(ctx, foreign.c_str(), buf, sizeof(buf)));
            RADOS_CALL(rados_write_full(ctx, visible.name().c_str(), buf, sizeof(buf)));

            std::vector<std::string> listed;
            for (const auto& o : RadosNamespace(pool.name(), nspace).listObjects()) {
                listed.push_back(o.name());
            }
            EXPECT(std::find(listed.begin(), listed.end(), "visible") != listed.end());
            EXPECT(std::find(listed.begin(), listed.end(), foreign) == listed.end());

            visible.ensureDestroyed();
            RADOS_CALL(rados_remove(ctx, foreign.c_str()));
        }

        // malformed URIs are reported as such
        EXPECT_THROWS_AS(RadosObject(URI("rados:pool/name")), eckit::UserError);
        EXPECT_THROWS_AS(RadosObject(URI("rados:pool/ns/name/more")), eckit::UserError);
        EXPECT_THROWS_AS(RadosNamespace(URI("rados:pool")), eckit::UserError);
        EXPECT_THROWS_AS(RadosPool(URI("rados:pool/ns")), eckit::UserError);
    }

    SECTION("removeAll removes every part, also when some are already gone") {

        const std::string data(100, 'p');

        RadosObject obj(pool.name(), nspace, "remove-all");

        std::unique_ptr<DataHandle> h(obj.multipartWriteHandle(16));
        h->openForWrite(0);
        h->write(data.data(), data.size());
        h->close();

        EXPECT(RadosObject(obj, 6).exists());
        EXPECT_NOT(RadosObject(obj, 7).exists());

        RadosObject(obj, 3).ensureDestroyed();

        obj.ensureAllDestroyed();

        for (size_t i = 0; i < 7; ++i) {
            EXPECT_NOT(RadosObject(obj, i).exists());
        }

        // must not throw for a missing object
        obj.ensureAllDestroyed();
    }

    SECTION("removeAll removes an object without parts") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "remove-single");

        RadosHandle h(obj);
        h.openForWrite(0);
        h.write(buf, sizeof(buf));
        h.close();

        obj.ensureAllDestroyed();
        EXPECT_NOT(obj.exists());
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

    SECTION("RadosAsyncHandle openForWrite empties an existing object") {

        const char buf[] = "abcdefghijklmnopqrstuvwxyz";

        RadosObject obj(pool.name(), nspace, "async-truncate");

        RadosAsyncHandle h(obj);
        h.openForWrite(0);
        h.write(buf, 26);
        h.close();
        EXPECT(h.size() == Length(26));

        // nothing written: the object exists and is empty
        h.openForWrite(0);
        h.close();
        EXPECT(obj.exists());
        EXPECT(h.size() == Length(0));

        // a first write at an offset leaves nothing of the previous content
        h.openForWrite(0);
        h.write(buf, 26);
        h.close();
        h.openForWrite(0);
        EXPECT(h.seek(2) == Offset(2));
        h.write(buf, 4);
        h.write(buf, 4);
        h.close();
        EXPECT(h.size() == Length(10));

        char out[16] = {};
        h.openForRead();
        EXPECT(h.read(out, sizeof(out)) == 10);
        h.close();
        EXPECT(std::string(out + 2, 8) == "abcdabcd");
        EXPECT(out[0] == 0);
        EXPECT(out[1] == 0);

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

    SECTION("Opening a handle on a missing pool fails and leaves it closed") {

        RadosObject obj("eckit-test-no-such-pool", nspace, "missing-pool");

        {
            RadosHandle h(obj);
            EXPECT_THROWS_AS(h.openForWrite(0), RadosEntityNotFoundException);
            EXPECT_THROWS_AS(h.openForRead(), RadosEntityNotFoundException);
        }
        {
            RadosAsyncHandle h(obj);
            EXPECT_THROWS_AS(h.openForWrite(0), RadosEntityNotFoundException);
        }
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
