// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/AIOHandle.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

// size is prime number 89
const char tbuf[] = "74e1feb8d0b1d328cbea63832c2dcfb2b4fa1adfeb8d0b1d328cb53d50e63a50fba73f0151028a695a238ff0";

class TestAIO {
public:

    TestAIO() {
        std::string base = Resource<std::string>("$TMPDIR", "/tmp");
        path_            = PathName::unique(base + "/file") + ".dat";
        reference_       = PathName::unique(base + "/reference") + ".dat";

        std::unique_ptr<DataHandle> fh(reference_.fileHandle());
        auto close = closer(*fh);
        writeTo(*fh);
    }

    size_t writeTo(DataHandle& dh) {
        long sz = sizeof(tbuf);

        dh.openForWrite(0);

        size_t nblocks = 30 * 1024 * 1024 / sz;
        size_t total   = 0;

        for (size_t i = 0; i < nblocks; ++i) {
            total += dh.write(tbuf, sz);
        }

        dh.close();

        return total;
    }

    bool verify() {
        std::unique_ptr<DataHandle> fh(path_.fileHandle());
        std::unique_ptr<DataHandle> rh(reference_.fileHandle());
        return rh->compare(*fh);
    }

    ~TestAIO() {
        path_.unlink();
        reference_.unlink();
    }

    PathName path_;
    PathName reference_;
};

CASE("Write to a new file") {

    TestAIO test;

    SECTION("Single write") {
        std::unique_ptr<DataHandle> rh(test.reference_.fileHandle());
        std::unique_ptr<DataHandle> aioh(new AIOHandle(test.path_));
        rh->saveInto(*aioh);
        EXPECT(test.verify());
    }

    SECTION("Multiple writes write") {
        std::unique_ptr<DataHandle> aioh(new AIOHandle(test.path_));
        test.writeTo(*aioh);
        EXPECT(test.verify());
    }
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
