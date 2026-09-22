// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosReadHandle.h"
#include "eckit/io/rados/RadosWriteHandle.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

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

    RadosWriteHandle h("mars:largeFile", 0);
    h.openForWrite(size);

    timer.start();
    h.write(buf, size);
    h.close();
    timer.stop();
    std::cout << " - write rate " << Bytes(size, timer) << std::endl;

    RadosReadHandle g("mars:largeFile");
    std::cout << "Size is " << g.openForRead() << std::endl;

    timer.start();
    g.read(buf, size);
    g.close();
    timer.stop();
    std::cout << " - read rate " << Bytes(size, timer) << std::endl;

    RadosCluster::instance().remove(RadosObject("mars:largeFile"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
