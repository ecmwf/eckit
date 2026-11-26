// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <rados/librados.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "RadosTestUtils.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/rados/RadosAsyncHandle.h"
#include "eckit/io/rados/RadosCall.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObject.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

namespace {

const char* const missingPool = "eckit-test-no-such-pool";

bool contains(const std::vector<std::string>& names, const std::string& name) {
    return std::find(names.begin(), names.end(), name) != names.end();
}

std::vector<std::string> names(const std::vector<RadosObject>& objects) {
    std::vector<std::string> res;
    for (const auto& o : objects) {
        res.push_back(o.name());
    }
    return res;
}

Length sizeOf(const RadosObject& object) {
    return RadosCluster::instance().size(object);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("Pools, namespaces and names") {

    RadosTestSpace space("test_handle");

    const RadosPool& pool     = space.pool();
    const std::string& nspace = space.nspace();

    SECTION("ensureCreated accepts an existing pool") {
        EXPECT(pool.exists());
        pool.ensureCreated();
        EXPECT(pool.exists());
        EXPECT_NOT(RadosPool(missingPool).exists());
        EXPECT_THROWS_AS(RadosPool(missingPool).destroy(), eckit::NotFound);
        RadosPool(missingPool).ensureDestroyed();
    }

    SECTION("the empty namespace is the default namespace") {

        const RadosObject inDefault(pool.name(), "", "eckit-test-default-ns");
        const RadosObject inNamed = space.object("eckit-test-named-ns");

        writeAll(inDefault.dataHandle(), "default");
        writeAll(inNamed.dataHandle(), "named");

        EXPECT(inDefault.exists());
        EXPECT_NOT(space.object("eckit-test-default-ns").exists());

        // each namespace only lists its own objects
        const auto inDefaultNs = names(RadosNamespace(pool.name(), "").listObjects());
        EXPECT(contains(inDefaultNs, "eckit-test-default-ns"));
        EXPECT_NOT(contains(inDefaultNs, "eckit-test-named-ns"));

        const auto inNamedNs = names(RadosNamespace(pool.name(), nspace).listObjects());
        EXPECT(contains(inNamedNs, "eckit-test-named-ns"));
        EXPECT_NOT(contains(inNamedNs, "eckit-test-default-ns"));

        const auto namespaces = pool.listNamespaces();
        EXPECT(contains(namespaces, ""));
        EXPECT(contains(namespaces, nspace));

        // the default namespace has no URI form
        EXPECT_THROWS_AS(inDefault.uri(), eckit::UserError);
        EXPECT_THROWS_AS(RadosNamespace(pool.name(), "").uri(), eckit::UserError);
        EXPECT(inDefault.str() == pool.name() + "//eckit-test-default-ns");

        inDefault.ensureDestroyed();
        EXPECT_NOT(inDefault.exists());
    }

    SECTION("names which would address something else are refused") {

        // a URI would turn them into another name, or librados would cut them short
        for (const std::string bad : {std::string("entry#1"), std::string("entry?1"), std::string("x/../y"),
                                      std::string("."), std::string(".."), std::string("a\0b", 3)}) {
            EXPECT_THROWS_AS(RadosObject(pool.name(), nspace, bad), eckit::UserError);
            EXPECT_THROWS_AS(RadosNamespace(pool.name(), bad), eckit::UserError);
            EXPECT_THROWS_AS(RadosPool{bad}, eckit::UserError);
        }
        EXPECT_THROWS_AS(RadosObject(URI("rados:pool/ns/..")), eckit::UserError);

        // other punctuation is fine and survives the URI
        for (const std::string good : {"a b", "a:b", "a%20b", "a.b", "a;b", "..."}) {
            const RadosObject o = space.object(good);
            EXPECT(RadosObject(URI(o.uri().asString())).str() == o.str());
        }
    }

    SECTION("an object with such a name, written by another tool, is left out of a listing") {

        const std::string foreign = "written#elsewhere";
        const RadosObject visible = space.object("visible");
        const rados_ioctx_t ctx   = RadosCluster::instance().ioCtx(visible);

        RADOS_CALL(rados_write_full(ctx, foreign.c_str(), "x", 1));
        writeAll(visible.dataHandle(), "x");

        const auto listed = names(RadosNamespace(pool.name(), nspace).listObjects());
        EXPECT(contains(listed, "visible"));
        EXPECT_NOT(contains(listed, foreign));

        RADOS_CALL(rados_remove(ctx, foreign.c_str()));
    }

    SECTION("URIs name pools, namespaces and objects") {

        const RadosObject obj = space.object("uri-object");

        const URI uri = obj.uri();
        EXPECT(uri.scheme() == "rados");
        EXPECT(uri.asString() == "rados:" + pool.name() + "/" + nspace + "/uri-object");
        EXPECT(RadosObject(uri).str() == obj.str());
        EXPECT(RadosObject(URI(uri.asString())).str() == obj.str());

        EXPECT(RadosNamespace(obj.nspace().uri()).str() == obj.nspace().str());
        EXPECT(RadosPool(pool.uri()).name() == pool.name());

        EXPECT_THROWS_AS(RadosObject(URI("rados:pool/name")), eckit::UserError);
        EXPECT_THROWS_AS(RadosObject(URI("rados:pool/ns/name/more")), eckit::UserError);
        EXPECT_THROWS_AS(RadosNamespace(URI("rados:pool")), eckit::UserError);
        EXPECT_THROWS_AS(RadosPool(URI("rados:pool/ns")), eckit::UserError);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("RadosHandle") {

    RadosTestSpace space("test_handle");

    const std::string data = someData(26);
    const RadosObject obj  = space.object("object");

    SECTION("writes and reads an object") {
        writeAll(obj.dataHandle(), data);
        EXPECT(obj.exists());
        EXPECT(sizeOf(obj) == Length(26));
        EXPECT(readAll(obj.dataHandle()) == data);

        obj.ensureDestroyed();
        EXPECT_NOT(obj.exists());
        obj.ensureDestroyed();
    }

    SECTION("openForWrite empties an existing object") {

        RadosHandle h(obj);

        writeAll(obj.dataHandle(), data);

        // nothing written: the object exists and is empty
        h.openForWrite(0);
        EXPECT(h.write(data.data(), 0) == 0);
        h.close();
        EXPECT(obj.exists());
        EXPECT(h.size() == Length(0));

        // a shorter rewrite leaves nothing of the previous content
        writeAll(obj.dataHandle(), data);
        writeAll(obj.dataHandle(), data.substr(0, 4));
        EXPECT(readAll(obj.dataHandle()) == data.substr(0, 4));
    }

    SECTION("seeks and rewinds") {

        RadosHandle h(obj);

        // previous, longer content
        writeAll(obj.dataHandle(), data + data);

        // seek before the first write is honoured, and nothing of the previous content is left
        h.openForWrite(0);
        EXPECT(h.seek(4) == Offset(4));
        h.write(data.data(), 26);
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
    }

    SECTION("refuses a write larger than the cluster accepts") {

        const long tooLong = static_cast<long>(RadosCluster::instance().maxWriteSize()) + 1;

        RadosHandle h(obj);
        h.openForWrite(0);
        // the size is checked before the data is looked at
        EXPECT_THROWS_AS(h.write(data.data(), tooLong), eckit::BadValue);
        h.close();
    }

    SECTION("fails to open on a missing pool or object, and stays closed") {

        const RadosObject noPool(missingPool, space.nspace(), "object");
        {
            RadosHandle h(noPool);
            EXPECT_THROWS_AS(h.openForWrite(0), eckit::NotFound);
            EXPECT_THROWS_AS(h.openForRead(), eckit::NotFound);
        }
        {
            RadosAsyncHandle h(noPool);
            EXPECT_THROWS_AS(h.openForWrite(0), eckit::NotFound);
        }
        {
            RadosHandle h(obj);
            EXPECT_THROWS_AS(h.openForRead(), eckit::NotFound);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("RadosAsyncHandle") {

    RadosTestSpace space("test_handle");

    const RadosObject obj = space.object("async");

    SECTION("close waits for the writes") {

        const std::string chunk(64 * 1024, 'x');

        RadosAsyncHandle h(obj);
        h.openForWrite(0);
        for (int i = 0; i < 8; ++i) {
            h.write(chunk.data(), chunk.size());
        }
        h.close();

        EXPECT(sizeOf(obj) == Length(8 * chunk.size()));
    }

    SECTION("flushes when the limit on pending data is reached") {

        const std::string chunk(4096, 'y');

        RadosAsyncHandle h(obj, 1024);
        h.openForWrite(0);
        for (int i = 0; i < 4; ++i) {
            h.write(chunk.data(), chunk.size());
        }
        h.close();

        EXPECT(sizeOf(obj) == Length(4 * chunk.size()));
    }

    SECTION("openForWrite empties an existing object") {

        const std::string data = someData(26);

        writeAll(obj.dataHandle(RadosWriteMode::Async), data);
        EXPECT(sizeOf(obj) == Length(26));

        // nothing written: the object exists and is empty
        writeAll(obj.dataHandle(RadosWriteMode::Async), "");
        EXPECT(obj.exists());
        EXPECT(sizeOf(obj) == Length(0));

        // a first write at an offset leaves nothing of the previous content
        writeAll(obj.dataHandle(RadosWriteMode::Async), data);
        RadosAsyncHandle h(obj);
        h.openForWrite(0);
        EXPECT(h.seek(2) == Offset(2));
        h.write(data.data(), 4);
        h.write(data.data(), 4);
        h.close();

        EXPECT(readAll(obj.dataHandle()) == std::string("\0\0", 2) + "abcdabcd");
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Multipart objects") {

    RadosTestSpace space("test_handle");

    const std::string data = someData(100);
    const RadosObject obj  = space.object("multi");
    const RadosMultiObject multi(obj);

    SECTION("are written in parts and read back") {

        // 7 parts of at most 16 bytes
        writeAll(obj.multipartWriteHandle(16), data);

        EXPECT(sizeOf(obj) == Length(16));
        EXPECT(sizeOf(multi.part(6)) == Length(4));
        EXPECT_NOT(multi.part(7).exists());

        const auto layout = multi.layout();
        EXPECT(layout);
        EXPECT(layout->length == Length(100));
        EXPECT(layout->parts == 7);
        EXPECT(layout->maxPartSize == Length(16));

        EXPECT(readAll(obj.multipartReadHandle()) == data);
    }

    SECTION("can end exactly at the end of a part") {
        writeAll(obj.multipartWriteHandle(16), data.substr(0, 32));
        EXPECT(multi.part(1).exists());
        EXPECT_NOT(multi.part(2).exists());
        EXPECT(readAll(obj.multipartReadHandle()) == data.substr(0, 32));
    }

    SECTION("can be empty") {
        writeAll(obj.multipartWriteHandle(16), "");
        EXPECT(obj.exists());
        EXPECT(multi.layout()->parts == 1);
        EXPECT(readAll(obj.multipartReadHandle()).empty());
    }

    SECTION("are written asynchronously, also with more parts than are open at a time") {

        // 13 parts, at most 3 of them open, 4 KiB of pending data each
        std::unique_ptr<DataHandle> h(obj.multipartWriteHandle(8, RadosWriteMode::Async, 4096, 3));
        h->openForWrite(0);
        h->write(data.data(), 50);
        h->flush();
        h->write(data.data() + 50, 50);
        h->close();

        EXPECT(multi.part(12).exists());
        EXPECT(readAll(obj.multipartReadHandle()) == data);
    }

    SECTION("take several asynchronous writes per part") {

        const std::string chunk(4096, 'z');

        std::unique_ptr<DataHandle> h(obj.multipartWriteHandle(10000, RadosWriteMode::Async));
        h->openForWrite(0);
        for (int i = 0; i < 5; ++i) {
            h->write(chunk.data(), chunk.size());
        }
        h->close();

        EXPECT(readAll(obj.multipartReadHandle()) == std::string(5 * chunk.size(), 'z'));
    }

    SECTION("split a write larger than the maximum write size") {

        const std::string large(static_cast<size_t>(RadosCluster::instance().maxWriteSize()) + 1024 * 1024, 'w');

        writeAll(obj.multipartWriteHandle(), large);

        EXPECT(sizeOf(obj) == Length(large.size()));
    }

    SECTION("lose the parts of a longer previous version when rewritten") {

        writeAll(obj.multipartWriteHandle(16), data);
        EXPECT(multi.part(6).exists());

        writeAll(obj.multipartWriteHandle(16), data.substr(0, 20));
        EXPECT(multi.part(1).exists());
        for (size_t i = 2; i < 7; ++i) {
            EXPECT_NOT(multi.part(i).exists());
        }

        EXPECT(readAll(obj.multipartReadHandle()) == data.substr(0, 20));
    }

    SECTION("are read from any position") {

        writeAll(obj.multipartWriteHandle(16), data);

        char out[128];

        RadosMultiObjReadHandle g(obj);
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

        // a handle opened again sees an object which was replaced in between
        writeAll(obj.multipartWriteHandle(8), data.substr(0, 20));
        EXPECT(g.size() == Length(20));
        EXPECT(g.openForRead() == Length(20));
        EXPECT(g.read(out, sizeof(out)) == 20);
        EXPECT(std::string(out, 20) == data.substr(0, 20));
        g.close();
    }

    SECTION("cannot be read if missing, or not written in parts") {

        RadosMultiObjReadHandle g(obj);
        EXPECT_THROWS_AS(g.openForRead(), eckit::NotFound);
        EXPECT_NOT(multi.layout());

        writeAll(obj.dataHandle(), data);
        EXPECT_NOT(multi.layout());
        EXPECT_THROWS_AS(g.openForRead(), eckit::RadosException);
    }

    SECTION("are removed with all their parts, also when some are already gone") {

        writeAll(obj.multipartWriteHandle(16), data);
        EXPECT(multi.part(6).exists());

        multi.part(3).ensureDestroyed();

        EXPECT(multi.remove());
        for (size_t i = 0; i < 7; ++i) {
            EXPECT_NOT(multi.part(i).exists());
        }

        // nothing left to remove
        EXPECT_NOT(multi.remove());
        obj.ensureAllDestroyed();
    }

    SECTION("removal also takes an object which was not written in parts") {
        writeAll(obj.dataHandle(), data);
        obj.ensureAllDestroyed();
        EXPECT_NOT(obj.exists());
    }

    SECTION("name their parts after the object") {
        EXPECT(multi.part(0).str() == obj.str());
        EXPECT(multi.part(3).name() == "multi;part-3");

        EXPECT_NOT(obj.isPart());
        EXPECT_NOT(multi.part(0).isPart());
        EXPECT(multi.part(1).isPart());
        EXPECT(multi.part(12).isPart());

        EXPECT_NOT(space.object("a;part-").isPart());
        EXPECT_NOT(space.object("a;part-x").isPart());
        EXPECT_NOT(space.object(";part-1").isPart());
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Ranges and URIs") {

    RadosTestSpace space("test_handle");

    const std::string data   = someData(100);
    const RadosObject single = space.object("single");
    const RadosObject multi  = space.object("multi");

    writeAll(single.dataHandle(), data);
    writeAll(multi.multipartWriteHandle(16), data);

    SECTION("a range of an object is read, rewound and read again") {

        for (DataHandle* handle : {single.rangeReadHandle(13, 40), multi.multipartRangeReadHandle(13, 40)}) {

            std::unique_ptr<DataHandle> g(handle);
            char out[128];

            EXPECT(g->openForRead() == Length(40));
            EXPECT(g->read(out, 10) == 10);
            EXPECT(std::string(out, 10) == data.substr(13, 10));
            EXPECT(g->read(out, sizeof(out)) == 30);
            EXPECT(std::string(out, 30) == data.substr(23, 30));
            EXPECT(g->read(out, sizeof(out)) == 0);

            g->rewind();
            EXPECT(g->read(out, 10) == 10);
            EXPECT(std::string(out, 10) == data.substr(13, 10));
            g->close();

            // opening the handle again starts from the beginning of the range
            EXPECT(g->openForRead() == Length(40));
            EXPECT(g->read(out, sizeof(out)) == 40);
            EXPECT(std::string(out, 40) == data.substr(13, 40));
            g->close();
        }
    }

    SECTION("a URI writes and reads an object") {

        const URI uri = space.object("by-uri").uri();

        EXPECT_NOT(uri.exists());
        writeAll(uri.newWriteHandle(), data);
        EXPECT(uri.exists());
        EXPECT(readAll(uri.newReadHandle()) == data);
    }

    SECTION("a URI reads the whole of a multipart object") {
        EXPECT(readAll(multi.uri().newReadHandle()) == data);
    }

    SECTION("a URI reads ranges of an object") {

        const OffsetList offsets = {Offset(5), Offset(50)};
        const LengthList lengths = {Length(10), Length(20)};

        const std::string expected = data.substr(5, 10) + data.substr(50, 20);

        EXPECT(readAll(single.uri().newReadHandle(offsets, lengths)) == expected);
        EXPECT(readAll(multi.uri().newReadHandle(offsets, lengths)) == expected);
    }

    SECTION("a URI to something missing fails when the handle is opened") {

        std::unique_ptr<DataHandle> noObject(space.object("none").uri().newReadHandle());
        EXPECT_THROWS_AS(noObject->openForRead(), eckit::NotFound);

        std::unique_ptr<DataHandle> noPool(RadosObject(missingPool, space.nspace(), "none").uri().newReadHandle());
        EXPECT_THROWS_AS(noPool->openForRead(), eckit::NotFound);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
