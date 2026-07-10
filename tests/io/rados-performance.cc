/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cassert>
#include <cstdio>
#include <iostream>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
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

    RadosPool pool("mars");
    /// @todo: auto pool destroyer
    pool.ensureCreated();

    RadosObject obj(pool.name(), "default", "largeFile");

    RadosMultiObjWriteHandle h(obj, false, 0);
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

    obj.ensureDestroyed();

    pool.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return eckit::testing::run_tests(argc, argv);
}
