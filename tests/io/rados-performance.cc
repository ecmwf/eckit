// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <cstdio>
#include <iostream>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
#ifndef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
#include "eckit/io/rados/RadosNamespace.h"
#endif
#include "RadosTestUtils.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Test rados performance") {

    eckit::Timer timer;
    size_t size;
    DataHandle* dh;

    eckit::PathName path("t.grib");
    size = path.size();
    eckit::Buffer buf(size);
    dh = path.fileHandle();
    dh->openForRead();
    dh->read(buf, size);
    dh->close();

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    std::string pool_name = "test_performance";
    std::string nspace    = "default";
    RadosPool pool(pool_name);
    pool.ensureDestroyed();
    pool.ensureCreated();
#else
    std::string pool_name;
    std::string nspace = "performance";
    pool_name          = eckit::Resource<std::string>("eckitRadosTestPool;$ECKIT_RADOS_TEST_POOL", pool_name);
    if (pool_name.empty()) {
        throw eckit::UserError("The RADOS tests need the name of an existing pool in ECKIT_RADOS_TEST_POOL", Here());
    }
    RadosPool pool(pool_name);
#endif

    RadosObject obj(pool.name(), nspace, "largeFile");

    RadosMultiObjWriteHandle h(obj);
    h.openForWrite(size);

    timer.start();
    h.write(buf, size);
    h.close();
    timer.stop();
    std::cout << " - write rate " << Bytes(size, timer) << std::endl;

    RadosMultiObjReadHandle g(obj);
    std::cout << "Size is " << g.openForRead() << std::endl;

    timer.start();
    g.read(buf, size);
    g.close();
    timer.stop();
    std::cout << " - read rate " << Bytes(size, timer) << std::endl;

    obj.ensureAllDestroyed();

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    pool.destroy();
#else
    RadosNamespace ns(pool_name, nspace);
    ns.destroy();
#endif
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    int ret = -1;
    try {
        ret = eckit::testing::run_tests(argc, argv);
    }
    catch (...) {
        eckit::Log::error() << "RADOS performance test terminated with an exception" << std::endl;
    }

#ifdef eckit_HAVE_RADOS_TESTS_MANAGE_POOLS
    eckit::test::cleanupRados("test_performance", "default");
#else
    eckit::test::cleanupRados(eckit::test::configuredRadosTestPool(), "performance");
#endif

    return ret;
}
