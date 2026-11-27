// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstddef>
#include <iostream>
#include <memory>

#include "RadosTestUtils.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Timer.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Test rados performance") {

    RadosTestSpace space("test_performance");

    const eckit::PathName path("t.grib");
    const size_t size = path.size();
    eckit::Buffer buf(size);
    {
        std::unique_ptr<DataHandle> file(path.fileHandle());
        file->openForRead();
        file->read(buf, size);
        file->close();
    }

    const RadosObject obj = space.object("largeFile");

    eckit::Timer timer;

    std::unique_ptr<DataHandle> h(obj.multipartWriteHandle());
    h->openForWrite(size);

    timer.start();
    h->write(buf, size);
    h->close();
    timer.stop();
    std::cout << " - write rate " << Bytes(size, timer) << std::endl;

    std::unique_ptr<DataHandle> g(obj.multipartReadHandle());
    std::cout << "Size is " << g->openForRead() << std::endl;

    timer.start();
    g->read(buf, size);
    g->close();
    timer.stop();
    std::cout << " - read rate " << Bytes(size, timer) << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return eckit::testing::run_tests(argc, argv);
}
