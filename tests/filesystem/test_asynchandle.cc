/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/AsyncHandle.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

class TestAsyncHandle {
public:
    void setup();
    void teardown();
    void test_write();
    void test_append();
    PathName path_;
};


void TestAsyncHandle::test_write() {
    std::unique_ptr<DataHandle> asynch(new AsyncHandle(path_.fileHandle()));

    asynch->openForWrite(0);

    const char buf[] = "74e1feb8d0b1d328cbea63832c2dcfb2b4fa1adf";

    asynch->write(buf, sizeof(buf));

    asynch->close();

    std::unique_ptr<DataHandle> fh(path_.fileHandle());

    fh->openForRead();

    Buffer buf2(1024);

    fh->read(buf2, buf2.size());
    fh->close();

    EXPECT(buf == std::string(buf2));
}


void TestAsyncHandle::test_append() {
    std::unique_ptr<DataHandle> asynch(new AsyncHandle(path_.fileHandle()));

    asynch->openForAppend(0);

    const char buf[] = "53d50e63a50fba73f0151028a695a238ff06491c";

    asynch->write(buf, sizeof(buf));

    asynch->close();

    std::unique_ptr<DataHandle> fh(path_.fileHandle());

    fh->openForRead();

    fh->seek(sizeof(buf));

    Buffer buf2(1024);

    fh->read(buf2, buf2.size());
    fh->close();

    EXPECT(buf == std::string(buf2));
}


void TestAsyncHandle::setup() {
    std::string base = Resource<std::string>("$TMPDIR", "/tmp");
    path_            = PathName::unique(base + "/lolo");
    path_ += ".dat";
}

void TestAsyncHandle::teardown() {
    path_.unlink();
}


CASE("test_asynchandle") {
    TestAsyncHandle test;
    test.setup();
    test.test_write();
    test.test_append();
    test.teardown();
}

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
